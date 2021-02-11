#ifndef _PREDICATES_INDEX_CACHE_MD_HPP_
#define _PREDICATES_INDEX_CACHE_MD_HPP_

#include <istream>
#include <memory>
#include <set>
#include <vector>

#include "PredicatesCacheMetadata.hpp"
#include "K2TreeMixed.hpp"

class PredicatesIndexCacheMD{
  PredicatesCacheMetadata metadata;

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
  PredicatesIndexCacheMD( std::unique_ptr<std::istream> &&is, K2TreeConfig k2tree_config );
  
  PredicatesIndexCacheMD(PredicatesIndexCacheMD &&other) noexcept;

  bool load_single_predicate(uint64_t predicate_index);
  K2TreeMixed & fetch_k2tree(uint64_t predicate_index);

  bool has_predicate(uint64_t predicate_index);
  bool has_predicate_active(uint64_t predicate_index);
  bool has_predicate_stored(uint64_t predicate_index);
  void add_predicate(uint64_t predicate_index);
  void insert_point(uint64_t subject_index, uint64_t predicate_index, uint64_t object_index);

  void sync_to_stream(std::ostream &os);
  void replace_istream(std::unique_ptr<std::istream> &&is);

  void discard_in_memory_predicate(uint64_t predicate_index);

  
};

#endif /* _PREDICATES_INDEX_CACHE_MD_HPP_ */