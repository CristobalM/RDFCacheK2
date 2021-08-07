#ifndef _PREDICATES_INDEX_CACHE_MD_HPP_
#define _PREDICATES_INDEX_CACHE_MD_HPP_

#include <istream>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

#include "K2TreeMixed.hpp"
#include "PredicateFetchResult.hpp"
#include "PredicatesCacheMetadata.hpp"

class PredicatesIndexCacheMD {
  PredicatesCacheMetadata metadata;
  std::mutex retrieval_mutex_1;
  std::mutex retrieval_mutex_2;

protected:
  std::unique_ptr<std::istream> is;

private:
  using predicates_map_t =
      std::unordered_map<uint64_t, std::unique_ptr<K2TreeMixed>>;

  predicates_map_t predicates;

  std::set<uint64_t> dirty_predicates;
  std::set<uint64_t> new_predicates;

  K2TreeConfig k2tree_config;

public:
  PredicatesIndexCacheMD(std::unique_ptr<std::istream> &&is);

  PredicatesIndexCacheMD(PredicatesIndexCacheMD &&other) noexcept;

  bool load_single_predicate(uint64_t predicate_index);
  PredicateFetchResult fetch_k2tree(uint64_t predicate_index);

  bool has_predicate(uint64_t predicate_index);
  bool has_predicate_active(uint64_t predicate_index);
  bool has_predicate_stored(uint64_t predicate_index);
  void add_predicate(uint64_t predicate_index);
  void insert_point(uint64_t subject_index, uint64_t predicate_index,
                    uint64_t object_index);

  void sync_to_stream(std::ostream &os);
  void replace_istream(std::unique_ptr<std::istream> &&is);

  void discard_in_memory_predicate(uint64_t predicate_index);
  K2TreeConfig get_config();
  const std::vector<uint64_t> &get_predicates_ids();

  const PredicatesCacheMetadata &get_metadata();
};

#endif /* _PREDICATES_INDEX_CACHE_MD_HPP_ */