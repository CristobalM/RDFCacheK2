#include "PredicatesCacheMetadata.hpp"
#include "serialization_util.hpp"

void PredicateMetadata::write_to_ostream(std::ostream &os) {
  write_u64(os, predicate_id);
  write_u64(os, tree_offset);
  write_u64(os, tree_size);
  write_u64(os, tree_size_in_memory);
  write_u32(os, priority);
  os.write(k2tree_hash.data(), k2tree_hash.size());
}

PredicateMetadata PredicateMetadata::read_from_istream(std::istream &is) {
  PredicateMetadata result;
  result.predicate_id = read_u64(is);
  result.tree_offset = read_u64(is);
  result.tree_size = read_u64(is);
  result.tree_size_in_memory = read_u64(is);
  result.priority = read_u32(is);
  is.read(result.k2tree_hash.data(), result.k2tree_hash.size());
  return result;
}

PredicatesCacheMetadata::PredicatesCacheMetadata(
    std::unordered_map<uint64_t, PredicateMetadata> &&metadata_map,
    std::vector<uint64_t> &&predicates_ids, K2TreeConfig config)
    : metadata_map(std::move(metadata_map)),
      predicates_ids(std::move(predicates_ids)), config(std::move(config)) {}

PredicatesCacheMetadata::PredicatesCacheMetadata(std::istream &is) {
  auto predicates_count = read_u64(is);
  config.read_from_istream(is);
  for (size_t i = 0; i < predicates_count; i++) {
    PredicateMetadata current = PredicateMetadata::read_from_istream(is);
    metadata_map[current.predicate_id] = current;
    predicates_ids.push_back(current.predicate_id);
  }
}

PredicatesCacheMetadata::PredicatesCacheMetadata(K2TreeConfig config)
    : config(config) {}

uint32_t PredicatesCacheMetadata::get_predicates_count() const {
  return predicates_ids.size();
}

const std::unordered_map<uint64_t, PredicateMetadata> &
PredicatesCacheMetadata::get_map() const {
  return metadata_map;
}

const std::vector<uint64_t> &PredicatesCacheMetadata::get_ids_vector() const {
  return predicates_ids;
}

K2TreeConfig PredicatesCacheMetadata::get_config() const { return config; }

void PredicatesCacheMetadata::write_to_ostream(std::ostream &os) {
  write_u64(os, predicates_ids.size());
  config.write_to_ostream(os);
  for (auto predicate_id : predicates_ids) {
    auto &metadata = metadata_map[predicate_id];
    metadata.write_to_ostream(os);
  }
}
