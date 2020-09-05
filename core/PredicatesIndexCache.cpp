//
// Created by Cristobal Miranda, 2020
//

#include <fstream>

#include "PredicatesIndexCache.hpp"

#include "predicates_index_cache.pb.h"
#include "serialization_util.hpp"

PredicatesIndexCache::PredicatesIndexCache() {}

void PredicatesIndexCache::feed_full_k2tree(
    proto_msg::CacheFeedFullK2TreeRequest &cache_feed_full_k2tree_request) {
  predicates_map[cache_feed_full_k2tree_request.predicate_index()] =
      std::make_unique<K2Tree>(cache_feed_full_k2tree_request);
}

bool PredicatesIndexCache::has_predicate(uint64_t predicate_index) {
  return predicates_map.find(predicate_index) != predicates_map.end();
}

K2Tree &PredicatesIndexCache::get_k2tree(uint64_t predicate_index) {
  return *predicates_map[predicate_index];
}

void PredicatesIndexCache::dump_to_file(const std::string &file_path) {
  std::fstream outfs(file_path,
                     std::ios::out | std::ios::trunc | std::ios::binary);

  auto first_pos = outfs.tellp();
  write_u32(outfs, 0); // place holder for max size
  write_u32(outfs, predicates_map.size());
  uint32_t max_size = 0;
  for (auto &hmap_item : predicates_map) {
    proto_msg::K2Tree k2tree;
    k2tree.set_predicate_index(hmap_item.first);
    k2tree.set_tree_depth(hmap_item.second->get_tree_depth());
    hmap_item.second->produce_proto(&k2tree);
    auto k2tree_serialized = k2tree.SerializeAsString();
    write_u32(outfs, k2tree_serialized.size());
    outfs.write(k2tree_serialized.c_str(), k2tree_serialized.size());
    if (k2tree_serialized.size() > max_size)
      max_size = k2tree_serialized.size();
  }

  auto curr = outfs.tellp();
  outfs.seekp(first_pos);
  write_u32(outfs, max_size);
  outfs.seekp(curr);
}

void PredicatesIndexCache::load_dump_file(const std::string &file_path) {
  std::ifstream ifstream(file_path, std::ifstream::binary);

  uint32_t max_sz = read_u32(ifstream);
  uint32_t map_sz = read_u32(ifstream);

  std::vector<char> buf(max_sz, 0);
  for (int i = 0; i < map_sz; i++) {
    uint32_t curr_sz = read_u32(ifstream);
    ifstream.read(buf.data(), curr_sz);
    proto_msg::K2Tree k2tree;
    k2tree.ParseFromArray(buf.data(), curr_sz);
    predicates_map[k2tree.predicate_index()] = std::make_unique<K2Tree>(k2tree);
  }
  ifstream.close();
}

void PredicatesIndexCache::add_predicate(uint64_t predicate_index) {
  predicates_map[predicate_index] = std::make_unique<K2Tree>(32, 1024);
}
