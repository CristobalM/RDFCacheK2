#include "PredicatesCacheMetadata.hpp"
#include "serialization_util.hpp"

PredicatesCacheMetadata::PredicatesCacheMetadata(){}

PredicatesCacheMetadata::PredicatesCacheMetadata(std::istream &is){
  auto predicates_count = read_u32(is);

  for(size_t i = 0; i < predicates_count; i++){
    PredicateMetadata current;
    current.predicate_id = read_u64(is);
    current.tree_offset = read_u64(is);
    current.tree_size = read_u64(is);
    current.priority = read_u32(is);
    metadata_map[current.predicate_id] = std::move(current);
    predicates_ids.push_back(current.predicate_id);
  }
}

uint32_t PredicatesCacheMetadata::get_predicates_count() const {
  return predicates_ids.size();
}

const std::unordered_map<uint64_t, PredicateMetadata> & PredicatesCacheMetadata::get_map(){
  return metadata_map;
}

const std::vector<uint64_t> &PredicatesCacheMetadata::get_ids_vector(){
  return predicates_ids;
}