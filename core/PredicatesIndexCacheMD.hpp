#ifndef _PREDICATES_INDEX_CACHE_MD_HPP_
#define _PREDICATES_INDEX_CACHE_MD_HPP_

#include <I_FileRWHandler.hpp>
#include <I_IStream.hpp>
#include <istream>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

#include "I_UpdateLoggerPCM.hpp"
#include "K2TreeMixed.hpp"
#include "PredicateFetchResult.hpp"
#include "PredicatesCacheMetadata.hpp"

class PredicatesIndexCacheMD {
  std::unique_ptr<I_FileRWHandler> file_handler;
  std::unique_ptr<I_IStream> is;

  PredicatesCacheMetadata metadata;
  std::mutex retrieval_mutex;
  std::mutex map_mutex;

private:
  using predicates_map_t =
      std::unordered_map<uint64_t, std::unique_ptr<K2TreeMixed>>;

  predicates_map_t predicates;

  std::set<uint64_t> dirty_predicates;
  std::set<uint64_t> new_predicates;

  K2TreeConfig k2tree_config;

  std::map<unsigned long, MemorySegment *> memory_segments_map;

  MemorySegment *full_memory_segment;

  I_UpdateLoggerPCM *update_logger;

public:
  explicit PredicatesIndexCacheMD(
      std::unique_ptr<I_FileRWHandler> &&file_handler);

  PredicatesIndexCacheMD(PredicatesIndexCacheMD &&other) noexcept;

  bool load_single_predicate(uint64_t predicate_index);
  PredicateFetchResult fetch_k2tree(uint64_t predicate_index);

  bool has_predicate(uint64_t predicate_index);
  bool has_predicate_active(uint64_t predicate_index);
  bool has_predicate_stored(uint64_t predicate_index);
  void add_predicate(uint64_t predicate_index);
  void insert_point(uint64_t subject_index, uint64_t predicate_index,
                    uint64_t object_index);

  // void replace_istream(std::unique_ptr<I_IStream> &&is);

  void discard_in_memory_predicate(uint64_t predicate_index);
  K2TreeConfig get_config();
  const std::vector<uint64_t> &get_predicates_ids();

  const PredicatesCacheMetadata &get_metadata();
  const PredicateMetadata *get_metadata_with_id(uint64_t predicate_id);
  void load_all_predicates();

  void set_update_logger(I_UpdateLoggerPCM *input_update_logger);

  bool is_stored_in_main_index(uint64_t predicate_id);
  bool is_stored_in_updates_log(uint64_t predicate_id);
  void mark_dirty(uint64_t predicate_id);

  void sync_to_persistent();

  PredicatesIndexCacheMD(const std::string &input_filename);

private:
  void sync_to_stream(std::ostream &os);
};

#endif /* _PREDICATES_INDEX_CACHE_MD_HPP_ */