#include <algorithm>

#include "PredicatesIndexCacheMD.hpp"
#include <serialization_util.hpp>

PredicatesIndexCacheMD::PredicatesIndexCacheMD(std::unique_ptr<std::istream> &&is, K2TreeConfig k2tree_config)
    : metadata(*is), is(std::move(is)), k2tree_config(k2tree_config) {}

PredicatesIndexCacheMD::PredicatesIndexCacheMD(PredicatesIndexCacheMD &&other)
: metadata(std::move(other.metadata)), is(std::move(other.is)), k2tree_config(other.k2tree_config)
{
  
}

bool PredicatesIndexCacheMD::load_single_predicate(uint64_t predicate_index) {
  if (metadata.get_map().find(predicate_index) == metadata.get_map().end()) {
    return false;
  }

  auto predicate_metadata = metadata.get_map().at(predicate_index);
  auto pos = is->tellg();
  is->seekg(predicate_metadata.tree_offset);
  predicates[predicate_metadata.predicate_id] =
      std::make_unique<K2TreeMixed>(K2TreeMixed::read_from_istream(*is));
  is->seekg(pos);
  return true;
}

K2TreeMixed &PredicatesIndexCacheMD::fetch_k2tree(uint64_t predicate_index) {
  if (predicates.find(predicate_index) == predicates.end() &&
      !load_single_predicate(predicate_index))
    throw std::runtime_error("Predicate with index " +
                             std::to_string(predicate_index) + " not found");

  return *predicates[predicate_index];
}

bool PredicatesIndexCacheMD::has_predicate(uint64_t predicate_index){
  return has_predicate_active(predicate_index) || has_predicate_stored(predicate_index);
}

bool PredicatesIndexCacheMD::has_predicate_active(uint64_t predicate_index){
  return predicates.find(predicate_index) != predicates.end();
}

bool PredicatesIndexCacheMD::has_predicate_stored(uint64_t predicate_index){
  return metadata.get_map().find(predicate_index) != metadata.get_map().end();
}

void PredicatesIndexCacheMD::add_predicate(uint64_t predicate_index){
  dirty_predicates.insert(predicate_index);
  predicates[predicate_index] = std::make_unique<K2TreeMixed>(k2tree_config);
  new_predicates.insert(predicate_index);
}

// TODO: fix to work with cache replacement
void PredicatesIndexCacheMD::insert_point(uint64_t subject_index, uint64_t predicate_index, uint64_t object_index){
  dirty_predicates.insert(predicate_index);

  auto predicate_active = has_predicate_active(predicate_index);
  auto predicate_stored = has_predicate_stored(predicate_index);

  if(predicate_stored && !predicate_stored){
    load_single_predicate(predicate_index);
  }
  else if(!predicate_stored && !predicate_active){
    add_predicate(predicate_index);
  }

  predicates[predicate_index]->insert(subject_index, object_index);
}

// Dont use the same stream as the stored, create a new one and then replace it with this
void PredicatesIndexCacheMD::sync_to_stream(std::ostream &os){
  std::vector<uint64_t> all_predicates(new_predicates.begin(), new_predicates.end());
  for(auto &it : metadata.get_map()){
    all_predicates.push_back(it.first);
  }

  std::sort(all_predicates.begin(), all_predicates.end());

  write_u64(os, all_predicates.size());

  std::unordered_map<uint64_t, PredicateMetadata> new_metadata_map;
  auto metadata_start = os.tellp();
  for(auto predicate_id : all_predicates){
    new_metadata_map[predicate_id] = PredicateMetadata{};
    new_metadata_map[predicate_id].write_to_ostream(os);
  }

  for(auto predicate_id : all_predicates){
    auto &meta = new_metadata_map[predicate_id];
    meta.predicate_id = predicate_id;
    meta.tree_offset = os.tellp();
    if(dirty_predicates.find(predicate_id) == dirty_predicates.end()){
      auto &current_md = metadata.get_map().at(predicate_id);
      meta.tree_size = current_md.tree_size;
      
      is->seekg(current_md.tree_offset);
      {
        std::vector<char> buf(meta.tree_size);
        is->read(buf.data(), buf.size());
        os.write(buf.data(), buf.size());
      }
    }
    else{
      predicates[predicate_id]->write_to_ostream(os);
      auto end_pos = static_cast<uint64_t>(os.tellp());
      meta.tree_size = end_pos - meta.tree_offset;
    }
  }

  auto last_pos = os.tellp();

  os.seekp(metadata_start);
  for(auto predicate_id : all_predicates){
    new_metadata_map[predicate_id].write_to_ostream(os);
  }
  os.seekp(last_pos);

  metadata = PredicatesCacheMetadata(std::move(new_metadata_map), std::move(all_predicates));
}

// Call after sync_to_stream
void PredicatesIndexCacheMD::replace_istream(std::unique_ptr<std::istream> &&is){
  this->is = std::move(is);
}

void PredicatesIndexCacheMD::discard_in_memory_predicate(uint64_t predicate_index){
  predicates.erase(predicate_index);
}
