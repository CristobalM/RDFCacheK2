//
// Created by Cristobal Miranda, 2020
//

#include <fstream>
#include <sstream>

#include "PredicatesIndexCache.hpp"

#include "predicates_index_cache.pb.h"
#include "serialization_util.hpp"

bool PredicatesIndexCache::has_predicate(uint64_t predicate_index) {
  return predicates_map.find(predicate_index) != predicates_map.end();
}

K2TreeMixed &PredicatesIndexCache::get_k2tree(uint64_t predicate_index) {
  return *predicates_map[predicate_index];
}

void PredicatesIndexCache::dump_to_file(const std::string &file_path) {
  std::fstream outfs(file_path,
                     std::ios::out | std::ios::trunc | std::ios::binary);
  dump_to_stream(outfs);
}

void PredicatesIndexCache::load_dump_file(const std::string &file_path) {
  std::ifstream ifstream(file_path, std::ios::binary | std::ios::out);

  auto loaded = from_stream(ifstream);
  *this = std::move(loaded);
}

void PredicatesIndexCache::add_predicate(uint64_t predicate_index) {
  predicates_map[predicate_index] = std::make_unique<K2TreeMixed>(32, 256, 10);
}

std::vector<unsigned long> PredicatesIndexCache::get_predicates_ids() {
  std::vector<unsigned long> result;
  for (auto iter = predicates_map.begin(); iter != predicates_map.end();
       iter++) {
    result.push_back(iter->first);
  }
  return result;
}

PredicatesIndexCache::PredicatesIndexCache(
    std::unordered_map<uint64_t, std::unique_ptr<K2TreeMixed>> &&predicates_map,
    PredicatesCacheMetadata &&metadata)
    : predicates_map(std::move(predicates_map)), metadata(std::move(metadata)) {
}

PredicatesIndexCache::predicates_map_t &
PredicatesIndexCache::get_predicates_map() {
  return predicates_map;
}

void PredicatesIndexCache::calculate_sizes() {
  for (auto it = predicates_map.begin(); it != predicates_map.end(); it++) {
    k2tree_sizes[it->first] = it->second->measure_in_memory_size().total_bytes;
  }
}

size_t PredicatesIndexCache::get_predicate_k2tree_size(
    uint64_t predicate_index) const {
  return k2tree_sizes.at(predicate_index);
}

void PredicatesIndexCache::dump_to_stream(std::ostream &ofs) {
  auto &predicates_ids = metadata.get_ids_vector();

  std::vector<unsigned long> tree_offsets;

  std::array<char, 28> metadata_unit_zeros{};

  write_u32(ofs, predicates_ids.size());

  auto begin_metadata = ofs.tellp();

  for (size_t i = 0; i < predicates_ids.size(); i++) {
    ofs.write(metadata_unit_zeros.data(), 28);
  }

  for (auto predicate_id : predicates_ids) {
    auto &k2tree = *predicates_map[predicate_id];
    auto start_offset = ofs.tellp();
    k2tree.write_to_ostream(ofs);
    tree_offsets.push_back(start_offset);
  }

  tree_offsets.push_back(ofs.tellp());

  ofs.seekp(begin_metadata);

  assert(predicates_ids.size() + 1 == tree_offsets.size());

  for (size_t i = 0; i < predicates_ids.size(); i++) {
    write_u64(ofs, predicates_ids[i]);
    write_u64(ofs, tree_offsets[i]);
    write_u64(ofs, tree_offsets[i + 1] - tree_offsets[i]);
    write_u32(ofs, 0); // TODO: write right value of the priority
  }

  ofs.seekp(
      tree_offsets[tree_offsets.size() - 1]); // put stream pointer at the end
}

PredicatesIndexCache PredicatesIndexCache::from_stream(std::istream &is) {
  std::unordered_map<uint64_t, std::unique_ptr<K2TreeMixed>> k2tree_map_result;

  auto metadata = PredicatesCacheMetadata(is);

  const auto &metadata_map = metadata.get_map();

  for (auto predicate_id : metadata.get_ids_vector()) {
    k2tree_map_result[predicate_id] =
        std::make_unique<K2TreeMixed>(K2TreeMixed::read_from_istream(is));
  }

  return PredicatesIndexCache(std::move(k2tree_map_result),
                              std::move(metadata));
}

PredicatesIndexCache PredicatesIndexCache::from_stream_subset(
    std::istream &is, const std::vector<uint64_t> &predicates_to_fetch) {
  std::unordered_map<uint64_t, std::unique_ptr<K2TreeMixed>> k2tree_map_result;

  auto metadata = PredicatesCacheMetadata(is);

  const auto &metadata_map = metadata.get_map();

  for (auto predicate_id : predicates_to_fetch) {
    if (metadata_map.find(predicate_id) == metadata_map.end())
      throw std::runtime_error("Can't find predicate_id " +
                               std::to_string(predicate_id) +
                               " on predicates map");
    auto predicate_metadata = metadata_map.at(predicate_id);
    is.seekg(predicate_metadata.tree_offset);
    k2tree_map_result[predicate_id] =
        std::make_unique<K2TreeMixed>(K2TreeMixed::read_from_istream(is));
  }

  return PredicatesIndexCache(std::move(k2tree_map_result),
                              std::move(metadata));
}
