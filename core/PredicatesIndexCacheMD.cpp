#include <algorithm>
#include <sstream>

#include "PredicatesIndexCacheMD.hpp"
#include <serialization_util.hpp>

#include <hashing.hpp>
#include <iostream>

PredicatesIndexCacheMD::PredicatesIndexCacheMD(
    std::unique_ptr<std::istream> &&is)
    : metadata(*is), is(std::move(is)), k2tree_config(metadata.get_config()) {}

PredicatesIndexCacheMD::PredicatesIndexCacheMD(
    PredicatesIndexCacheMD &&other) noexcept
    : metadata(std::move(other.metadata)), is(std::move(other.is)),
      k2tree_config(other.k2tree_config) {}

bool PredicatesIndexCacheMD::load_single_predicate(uint64_t predicate_index) {
  if (!has_predicate_stored(predicate_index))
    return false;

  std::lock_guard lg(retrieval_mutex);

  std::cerr << "retrieving predicate " << predicate_index << " from memory"
            << std::endl;

  const auto &predicate_metadata = get_metadata_with_id(predicate_index);

  auto pos = is->tellg();
  is->seekg(predicate_metadata.tree_offset);

  const auto &predicates_v = metadata.get_ids_vector();
  auto pos_it =
      std::find(predicates_v.begin(), predicates_v.end(), predicate_index);
  if (pos_it == predicates_v.end())
    throw std::runtime_error("predicate " + std::to_string(predicate_index) +
                             " is not within the list");
  pos_it++;

  size_t end_pos;
  if (pos_it == predicates_v.end()) {
    auto curr = is->tellg();
    is->seekg(0, std::ios::end);
    end_pos = is->tellg();
    is->seekg(curr);
  } else {

    if (!has_predicate_stored(*pos_it))
      throw std::runtime_error("predicate id " + std::to_string(*pos_it) +
                               " in list but not on metadata map");
    const auto &md = get_metadata_with_id(*pos_it);
    end_pos = md.tree_offset;
  }

  auto raw_k2tree_sz = end_pos - predicate_metadata.tree_offset;
  std::vector<char> raw_k2tree(raw_k2tree_sz, 0);

  is->read(raw_k2tree.data(), static_cast<std::streamsize>(raw_k2tree.size()));

  auto md5_calc = md5calc(raw_k2tree);

  if (md5_calc != predicate_metadata.k2tree_hash) {
    throw std::runtime_error("Calc hash differs from stored for predicate " +
                             std::to_string(predicate_metadata.predicate_id));
  }

  std::stringstream ss(
      std::string(raw_k2tree.begin(),
                  raw_k2tree.end())); // TODO: optimization using a custom
                                      // stream to avoid copying the data

  std::cout << "Loading predicate " << predicate_metadata.predicate_id << " ..."
            << std::endl;

  predicates[predicate_metadata.predicate_id] =
      std::make_unique<K2TreeMixed>(K2TreeMixed::read_from_istream(ss));
  is->seekg(pos);
  std::cout << "Loaded " << predicate_metadata.predicate_id << std::endl;

  return true;
}

PredicateFetchResult
PredicatesIndexCacheMD::fetch_k2tree(uint64_t predicate_index) {
  if (predicates.find(predicate_index) == predicates.end() &&
      !load_single_predicate(predicate_index)) {
    return PredicateFetchResult(false, nullptr);
  }
  return PredicateFetchResult(true, predicates[predicate_index].get());
}

bool PredicatesIndexCacheMD::has_predicate(uint64_t predicate_index) {
  return has_predicate_active(predicate_index) ||
         has_predicate_stored(predicate_index);
}

bool PredicatesIndexCacheMD::has_predicate_active(uint64_t predicate_index) {
  return predicates.find(predicate_index) != predicates.end();
}

bool PredicatesIndexCacheMD::has_predicate_stored(uint64_t predicate_index) {
  std::lock_guard lg(map_mutex);
  return metadata.get_map().find(predicate_index) != metadata.get_map().end();
}

void PredicatesIndexCacheMD::add_predicate(uint64_t predicate_index) {
  dirty_predicates.insert(predicate_index);
  predicates[predicate_index] = std::make_unique<K2TreeMixed>(k2tree_config);
  new_predicates.insert(predicate_index);
}

// TODO: fix to work with cache replacement
void PredicatesIndexCacheMD::insert_point(uint64_t subject_index,
                                          uint64_t predicate_index,
                                          uint64_t object_index) {
  dirty_predicates.insert(predicate_index);

  auto predicate_active = has_predicate_active(predicate_index);
  auto predicate_stored = has_predicate_stored(predicate_index);

  if (predicate_stored && !predicate_active) {
    load_single_predicate(predicate_index);
  } else if (!predicate_stored && !predicate_active) {
    add_predicate(predicate_index);
  }

  predicates[predicate_index]->insert(subject_index, object_index);
}

// Dont use the same stream as the stored, create a new one and then replace it
// with this
void PredicatesIndexCacheMD::sync_to_stream(std::ostream &os) {

  std::lock_guard lg(retrieval_mutex);

  std::vector<uint64_t> all_predicates(new_predicates.begin(),
                                       new_predicates.end());
  for (auto &it : metadata.get_map()) {
    all_predicates.push_back(it.first);
  }

  std::sort(all_predicates.begin(), all_predicates.end());

  write_u64(os, all_predicates.size());
  k2tree_config.write_to_ostream(os);

  std::unordered_map<uint64_t, PredicateMetadata> new_metadata_map;
  auto metadata_start = os.tellp();
  for (auto predicate_id : all_predicates) {
    new_metadata_map[predicate_id] = PredicateMetadata{};
    new_metadata_map[predicate_id].write_to_ostream(os);
  }

  for (auto predicate_id : all_predicates) {
    auto &meta = new_metadata_map[predicate_id];
    meta.predicate_id = predicate_id;
    meta.tree_offset = os.tellp();
    if (dirty_predicates.find(predicate_id) == dirty_predicates.end()) {
      auto &current_md = metadata.get_map().at(predicate_id);
      meta.tree_size = current_md.tree_size;
      meta.k2tree_hash = current_md.k2tree_hash;

      is->seekg(current_md.tree_offset);
      {
        std::vector<char> buf(meta.tree_size);
        is->read(buf.data(), buf.size());
        os.write(buf.data(), buf.size());
      }
    } else {
      std::stringstream ss;
      auto sz = predicates[predicate_id]->write_to_ostream(ss);
      auto k2tree_str = ss.str();
      auto md5_calc = md5calc(k2tree_str);
      meta.tree_size = sz;
      meta.k2tree_hash = std::move(md5_calc);
      os.write(k2tree_str.data(), k2tree_str.size());
    }
  }

  auto last_pos = os.tellp();

  os.seekp(metadata_start);
  for (auto predicate_id : all_predicates) {
    new_metadata_map[predicate_id].write_to_ostream(os);
  }
  os.seekp(last_pos);

  metadata = PredicatesCacheMetadata(std::move(new_metadata_map),
                                     std::move(all_predicates), k2tree_config);
}

// Call after sync_to_stream
void PredicatesIndexCacheMD::replace_istream(
    std::unique_ptr<std::istream> &&_is) {
  this->is = std::move(_is);
}

void PredicatesIndexCacheMD::discard_in_memory_predicate(
    uint64_t predicate_index) {
  std::lock_guard lg(retrieval_mutex);
  std::cerr << "removing predicate " << predicate_index << " from memory"
            << std::endl;
  predicates.erase(predicate_index);
}

K2TreeConfig PredicatesIndexCacheMD::get_config() { return k2tree_config; }

const std::vector<uint64_t> &PredicatesIndexCacheMD::get_predicates_ids() {
  return metadata.get_ids_vector();
}

const PredicatesCacheMetadata &PredicatesIndexCacheMD::get_metadata() {
  return metadata;
}
const PredicateMetadata &
PredicatesIndexCacheMD::get_metadata_with_id(uint64_t predicate_id) {
  std::lock_guard lg(map_mutex);
  const auto &metadata_map = metadata.get_map();
  return metadata_map.at(predicate_id);
}
