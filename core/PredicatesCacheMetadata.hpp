#ifndef _PREDICATES_CACHE_METADATA_HPP_
#define _PREDICATES_CACHE_METADATA_HPP_

#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>
#include <unordered_map>
#include <vector>

#include "K2TreeMixed.hpp"
struct PredicateMetadata {
  uint64_t predicate_id;
  uint64_t tree_offset;
  uint64_t tree_size; // serialized size
  uint64_t tree_size_in_memory;
  uint32_t priority;
  std::array<char, 16> k2tree_hash;

  void write_to_ostream(std::ostream &ostream);
  static PredicateMetadata read_from_istream(std::istream &is);
};

class PredicatesCacheMetadata {

  std::unordered_map<uint64_t, PredicateMetadata> metadata_map;
  std::vector<uint64_t> predicates_ids;
  K2TreeConfig config;

public:
  PredicatesCacheMetadata(
      std::unordered_map<uint64_t, PredicateMetadata> &&metadata_map,
      std::vector<uint64_t> &&predicates_ids, K2TreeConfig config);
  explicit PredicatesCacheMetadata(std::istream &is);
  explicit PredicatesCacheMetadata(K2TreeConfig config);

  uint32_t get_predicates_count() const;
  const std::unordered_map<uint64_t, PredicateMetadata> &get_map() const;
  const std::vector<uint64_t> &get_ids_vector() const;
  K2TreeConfig get_config() const;

  void write_to_ostream(std::ostream &os);
};

#endif /* _PREDICATES_CACHE_METADATA_HPP_ */
