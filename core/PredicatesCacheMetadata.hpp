#ifndef _PREDICATES_CACHE_METADATA_HPP_
#define _PREDICATES_CACHE_METADATA_HPP_

#include <unordered_map>
#include <cstdint>
#include <memory>
#include <istream>
#include <vector>

struct PredicateMetadata{
  uint64_t predicate_id;
  uint64_t tree_offset;
  uint64_t tree_size;
  uint32_t priority;
};

class PredicatesCacheMetadata{

  std::unordered_map<uint64_t, PredicateMetadata> metadata_map;
  std::vector<uint64_t> predicates_ids;

  public:
  PredicatesCacheMetadata();
  explicit PredicatesCacheMetadata(std::istream &is);

  uint32_t get_predicates_count() const;
  const std::unordered_map<uint64_t, PredicateMetadata> & get_map();
  const std::vector<uint64_t> &get_ids_vector();

};

#endif /* _PREDICATES_CACHE_METADATA_HPP_ */
