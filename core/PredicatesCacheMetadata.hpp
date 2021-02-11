#ifndef _PREDICATES_CACHE_METADATA_HPP_
#define _PREDICATES_CACHE_METADATA_HPP_

#include <unordered_map>
#include <cstdint>
#include <memory>
#include <istream>
#include <ostream>
#include <vector>

#include "K2TreeMixed.hpp"
struct PredicateMetadata{
  uint64_t predicate_id;
  uint64_t tree_offset;
  uint64_t tree_size;
  uint32_t priority;

  void write_to_ostream(std::ostream &ostream);
};

class PredicatesCacheMetadata{

  std::unordered_map<uint64_t, PredicateMetadata> metadata_map;
  std::vector<uint64_t> predicates_ids;
  K2TreeConfig config;

  public:
  PredicatesCacheMetadata(
    std::unordered_map<uint64_t, PredicateMetadata> &&metadata_map,
    std::vector<uint64_t> &&predicates_ids,
    K2TreeConfig config
    );
  explicit PredicatesCacheMetadata(std::istream &is);

  uint32_t get_predicates_count() const;
  const std::unordered_map<uint64_t, PredicateMetadata> & get_map();
  const std::vector<uint64_t> &get_ids_vector();
  K2TreeConfig get_config();

};

#endif /* _PREDICATES_CACHE_METADATA_HPP_ */
