//
// Created by Cristobal Miranda, 2020
//

#include <fstream>

#include "PredicatesIndexCache.hpp"

#include "predicates_index_cache.pb.h"

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
  proto_msg::PredicatesIndexCache predicates_index_cache_proto;
  for (auto &hmap_item : predicates_map) {
    proto_msg::K2Tree *k2tree = predicates_index_cache_proto.add_k2tree();
    k2tree->set_predicate_index(hmap_item.first);
    k2tree->set_tree_depth(hmap_item.second->get_tree_depth());
    hmap_item.second->produce_proto(k2tree);
  }

  std::cout << "PredicatesIndexCache String serialized size: " << predicates_index_cache_proto.SerializeAsString().size() << std::endl;

  std::fstream outfs(file_path, std::ios::out | std::ios::trunc | std::ios::binary);
  if(!predicates_index_cache_proto.SerializeToOstream(&outfs)){
    std::cerr << "Failed to serialize the predicates index cache" << std::endl;
  }


}

void PredicatesIndexCache::load_dump_file(const std::string &file_path) {
  std::ifstream ifstream(file_path, std::ifstream::binary);

  proto_msg::PredicatesIndexCache predicates_index_cache_proto;
  predicates_index_cache_proto.ParseFromIstream(&ifstream);

  for (int i = 0; i < predicates_index_cache_proto.k2tree_size(); i++) {
    const proto_msg::K2Tree &k2tree = predicates_index_cache_proto.k2tree(i);
    predicates_map[k2tree.predicate_index()] = std::make_unique<K2Tree>(k2tree);
  }

  ifstream.close();
}
void PredicatesIndexCache::add_predicate(uint64_t predicate_index) {
  predicates_map[predicate_index] = std::make_unique<K2Tree>(32);
}
