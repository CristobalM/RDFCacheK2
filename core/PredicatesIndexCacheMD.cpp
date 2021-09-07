#include <algorithm>
#include <sstream>

#include "PredicatesIndexCacheMD.hpp"
#include <serialization_util.hpp>

#include <MemoryManager.hpp>
#include <hashing.hpp>

PredicatesIndexCacheMD::PredicatesIndexCacheMD(
    std::unique_ptr<std::istream> &&is)
    : metadata(*is), is(std::move(is)), k2tree_config(metadata.get_config()),
      full_memory_segment(nullptr), update_logger(nullptr) {}

PredicatesIndexCacheMD::PredicatesIndexCacheMD(
    PredicatesIndexCacheMD &&other) noexcept
    : metadata(std::move(other.metadata)), is(std::move(other.is)),
      k2tree_config(other.k2tree_config),
      memory_segments_map(std::move(other.memory_segments_map)),
      full_memory_segment(other.full_memory_segment), update_logger(nullptr) {}

bool PredicatesIndexCacheMD::load_single_predicate(uint64_t predicate_index) {
  if (!has_predicate_stored(predicate_index))
    return false;

  std::lock_guard lg(retrieval_mutex);

  const auto &predicate_metadata = get_metadata_with_id(predicate_index);

  auto pos = is->tellg();
  is->seekg(predicate_metadata.tree_offset);

  auto *memory_segment = MemoryManager::instance().new_memory_segment(
      predicate_metadata.tree_size_in_memory);
  memory_segments_map[predicate_metadata.predicate_id] = memory_segment;

  predicates[predicate_metadata.predicate_id] = std::make_unique<K2TreeMixed>(
      K2TreeMixed::read_from_istream(*is, memory_segment));
  is->seekg(pos);

  if (update_logger) {
    update_logger->recover_predicate(predicate_index);
  }

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
  return predicates.find(predicate_index) != predicates.end() ||
         new_predicates.find(predicate_index) != new_predicates.end();
}

bool PredicatesIndexCacheMD::has_predicate_stored(uint64_t predicate_index) {
  std::lock_guard lg(map_mutex);
  return metadata.get_map().find(predicate_index) != metadata.get_map().end() ||
  new_predicates.find(predicate_index) != new_predicates.end();
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
      meta.tree_size_in_memory = current_md.tree_size_in_memory;

      is->seekg(current_md.tree_offset);
      {
        std::vector<char> buf(meta.tree_size);
        is->read(buf.data(), buf.size());
        os.write(buf.data(), buf.size());
      }
    } else {
      std::stringstream ss;
      auto &in_memory_tree = *predicates[predicate_id];
      auto stats = in_memory_tree.k2tree_stats();
      auto sz = in_memory_tree.write_to_ostream(ss);
      auto k2tree_str = ss.str();
      auto md5_calc = md5calc(k2tree_str);
      meta.tree_size = sz;
      meta.tree_size_in_memory = stats.total_bytes;
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
  predicates.erase(predicate_index);
  auto it = memory_segments_map.find(predicate_index);
  if (it != memory_segments_map.end()) {
    MemoryManager::instance().free_segment(it->second);
    memory_segments_map.erase(it);
  }
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
void PredicatesIndexCacheMD::load_all_predicates() {
  std::lock_guard lg(retrieval_mutex);

  size_t total_bytes = 0;
  auto &all_predicates = metadata.get_ids_vector();
  if (all_predicates.empty())
    return;
  for (auto predicate_id : all_predicates) {
    auto &md = metadata.get_map().at(predicate_id);
    total_bytes += md.tree_size_in_memory;
  }
  auto *mem_segment = MemoryManager::instance().new_memory_segment(total_bytes);
  full_memory_segment = mem_segment;
  auto first_offset = metadata.get_map().at(all_predicates[0]).tree_offset;
  is->seekg(first_offset);

  for (auto it = all_predicates.begin(); it != all_predicates.end(); it++) {
    /*
    size_t end_pos;
    auto next_it = std::next(it);
    if (next_it == all_predicates.end()) {
      auto curr = is->tellg();
      is->seekg(0, std::ios::end);
      end_pos = is->tellg();
      is->seekg(curr);
    } else {
      if (!has_predicate_stored(*next_it))
        throw std::runtime_error("predicate id " + std::to_string(*next_it) +
                                 " in list but not on metadata map");
      const auto &md = metadata.get_map().at(*next_it);
      end_pos = md.tree_offset;
    }
    const auto &curr_md = metadata.get_map().at(*it);

    auto raw_k2tree_sz = end_pos - curr_md.tree_offset;
    std::vector<char> raw_k2tree(raw_k2tree_sz, 0);

    is->read(raw_k2tree.data(),
             static_cast<std::streamsize>(raw_k2tree.size()));

    auto md5_calc = md5calc(raw_k2tree);

    if (md5_calc != curr_md.k2tree_hash) {
      throw std::runtime_error("hashes differ");
    }

    std::stringstream ss(std::string(raw_k2tree.begin(), raw_k2tree.end()));

    raw_k2tree.clear();
*/
    predicates[*it] = std::make_unique<K2TreeMixed>(
        K2TreeMixed::read_from_istream(*is, mem_segment));
  }
}
void PredicatesIndexCacheMD::set_update_logger(
    I_UpdateLoggerPCM *input_update_logger) {
  update_logger = input_update_logger;
}
