#include <algorithm>
#include <sstream>

#include "PredicatesIndexCacheMD.hpp"
#include <serialization_util.hpp>

#include <FileRWHandler.hpp>
#include <MemoryManager.hpp>
#include <hashing.hpp>

PredicatesIndexCacheMD::PredicatesIndexCacheMD(
    const std::string &input_filename)
    : PredicatesIndexCacheMD(std::make_unique<FileRWHandler>(input_filename)) {}

PredicatesIndexCacheMD::PredicatesIndexCacheMD(
    std::unique_ptr<I_FileRWHandler> &&file_handler)
    : file_handler(std::move(file_handler)),
      is(this->file_handler->get_reader(std::ios::binary)),
      metadata(is->get_istream()), k2tree_config(metadata.get_config()),
      full_memory_segment(nullptr), update_logger(nullptr) {}

PredicatesIndexCacheMD::PredicatesIndexCacheMD(
    PredicatesIndexCacheMD &&other) noexcept
    : file_handler(std::move(other.file_handler)), is(std::move(other.is)),
      metadata(std::move(other.metadata)), k2tree_config(other.k2tree_config),
      memory_segments_map(std::move(other.memory_segments_map)),
      full_memory_segment(other.full_memory_segment), update_logger(nullptr) {}

bool PredicatesIndexCacheMD::load_single_predicate(uint64_t predicate_index) {
  if (!is_stored_in_main_index(predicate_index) &&
      !is_stored_in_updates_log(predicate_index))
    return false;

  std::lock_guard lg(retrieval_mutex);

  if (is_stored_in_main_index(predicate_index)) {
    const auto *predicate_metadata = get_metadata_with_id(predicate_index);
    if (!predicate_metadata)
      return false;

    auto pos = is->tellg();
    is->seekg(predicate_metadata->tree_offset);

    auto *memory_segment = MemoryManager::instance().new_memory_segment(
        predicate_metadata->tree_size_in_memory);
    memory_segments_map[predicate_metadata->predicate_id] = memory_segment;

    predicates[predicate_metadata->predicate_id] =
        std::make_unique<K2TreeMixed>(
            K2TreeMixed::read_from_istream(is->get_istream(), memory_segment));
    is->seekg(pos);
  }

  if (update_logger && is_stored_in_updates_log(predicate_index)) {
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
  if (!has_predicate_active(predicate_index) &&
      is_stored_in_updates_log(predicate_index)) {
    add_predicate(predicate_index);
    update_logger->recover_predicate(predicate_index);
  }
  return PredicateFetchResult(true, predicates[predicate_index].get());
}

PredicateFetchResult
PredicatesIndexCacheMD::fetch_k2tree_if_loaded(uint64_t predicate_index) {
  auto it = predicates.find(predicate_index);
  if (it != predicates.end()) {
    return PredicateFetchResult(true, it->second.get());
  }

  if (has_predicate_stored(predicate_index)) {
    return PredicateFetchResult(true, nullptr);
  }

  return PredicateFetchResult(false, nullptr);
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
  auto stored_in_main_index = is_stored_in_main_index(predicate_index);
  if (stored_in_main_index)
    return true;
  auto stored_in_updates_log = is_stored_in_updates_log(predicate_index);
  return stored_in_updates_log;
  //  return is_stored_in_main_index(predicate_index) ||
  //         is_stored_in_updates_log(predicate_index);
}

void PredicatesIndexCacheMD::add_predicate(uint64_t predicate_index) {
  dirty_predicates.insert(predicate_index);
  auto new_k2tree = std::make_unique<K2TreeMixed>(k2tree_config);
  predicates[predicate_index] = std::move(new_k2tree);
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
        is->get_istream().read(buf.data(), buf.size());
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
/*
void PredicatesIndexCacheMD::replace_istream(
    std::unique_ptr<I_IStream> &&_is) {
  this->is = std::move(_is);
}
 */

void PredicatesIndexCacheMD::discard_in_memory_predicate(
    uint64_t predicate_index) {
  std::lock_guard lg(retrieval_mutex);
  predicates.erase(predicate_index);
  auto it = memory_segments_map.find(predicate_index);
  if (it != memory_segments_map.end()) {
    MemoryManager::instance().free_segment(it->second);
    memory_segments_map.erase(it);
  }
  auto it_new_predicates = new_predicates.find(predicate_index);
  if (it_new_predicates != new_predicates.end()) {
    new_predicates.erase(it_new_predicates);
  }
  auto it_dirty = dirty_predicates.find(predicate_index);
  if (it_dirty != dirty_predicates.end()) {
    dirty_predicates.erase(it_dirty);
  }
}

K2TreeConfig PredicatesIndexCacheMD::get_config() { return k2tree_config; }

const std::vector<uint64_t> &PredicatesIndexCacheMD::get_predicates_ids() {
  return metadata.get_ids_vector();
}

const PredicatesCacheMetadata &PredicatesIndexCacheMD::get_metadata() {
  return metadata;
}
const PredicateMetadata *
PredicatesIndexCacheMD::get_metadata_with_id(uint64_t predicate_id) {
  std::lock_guard lg(map_mutex);
  const auto &metadata_map = metadata.get_map();
  return &metadata_map.at(predicate_id);
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

  predicates.reserve(all_predicates.size() +
                     (unsigned long)((double)all_predicates.size() * .5));
  for (unsigned long predicate_id : all_predicates) {
    // commented code kept for future debugging
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
    predicates[predicate_id] = std::make_unique<K2TreeMixed>(
        K2TreeMixed::read_from_istream(is->get_istream(), mem_segment));
  }
}
void PredicatesIndexCacheMD::set_update_logger(
    I_UpdateLoggerPCM *input_update_logger) {
  update_logger = input_update_logger;
}
bool PredicatesIndexCacheMD::is_stored_in_main_index(uint64_t predicate_id) {
  return metadata.get_map().find(predicate_id) != metadata.get_map().end();
}
bool PredicatesIndexCacheMD::is_stored_in_updates_log(uint64_t predicate_id) {
  return update_logger && update_logger->has_predicate_stored(predicate_id);
}
void PredicatesIndexCacheMD::mark_dirty(uint64_t predicate_id) {
  dirty_predicates.insert(predicate_id);
}
void PredicatesIndexCacheMD::sync_to_persistent() {
  auto temp_writer =
      file_handler->get_writer_temp(std::ios::binary | std::ios::trunc);
  sync_to_stream(temp_writer->get_ostream());
  temp_writer->flush();
  is = nullptr; // closes the open file
  file_handler->commit_temp_writer();
  is = file_handler->get_reader(std::ios::binary);
}

void PredicatesIndexCacheMD::sync_logs_to_indexes() {
  if (!update_logger) {
    throw std::runtime_error("no update logger available");
  }
  update_logger->compact_logs();
  auto logger_predicates = update_logger->get_predicates();

  static constexpr auto threshold = 1'000'000'000UL; // 1GB

  std::set<unsigned long> loaded_predicates;

  unsigned long total_sz = 0;
  for (auto p : logger_predicates) {
    // we are only interested on saving non-loaded indexes in this step
    if (has_predicate_active(p))
      continue;
    // will automatically load updates from updates_logger
    auto fetched = fetch_k2tree(p);
    if (!fetched.exists())
      continue;

    loaded_predicates.insert(p);

    auto meta = get_metadata_with_id(p);
    total_sz += meta->tree_size_in_memory;

    if (total_sz >= threshold)
      clean_up_bulk_sync(loaded_predicates, total_sz);
  }
  if (total_sz > 0)
    clean_up_bulk_sync(loaded_predicates, total_sz);
}
void PredicatesIndexCacheMD::clean_up_bulk_sync(
    std::set<unsigned long> &loaded_predicates, unsigned long &total_sz) {
  sync_to_persistent();
  for (auto p : loaded_predicates) {
    discard_in_memory_predicate(p);
  }
  loaded_predicates.clear();
  total_sz = 0;
}
void PredicatesIndexCacheMD::full_sync_logs_and_memory_with_persistent() {
  sync_to_persistent();
  sync_logs_to_indexes();
  update_logger->clean_append_log();
}
