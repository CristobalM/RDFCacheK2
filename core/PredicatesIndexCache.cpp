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
      std::make_unique<K2TreeWMutex>(K2Tree(cache_feed_full_k2tree_request));
}

bool PredicatesIndexCache::has_predicate(uint64_t predicate_index) {
  return predicates_map.find(predicate_index) != predicates_map.end();
}

K2Tree &PredicatesIndexCache::get_k2tree(uint64_t predicate_index) {
  return predicates_map[predicate_index]->k2tree;
}

K2TreeWMutex &
PredicatesIndexCache::get_k2tree_wmutex(uint64_t predicate_index) {
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
    k2tree.set_tree_depth(hmap_item.second->k2tree.get_tree_depth());
    hmap_item.second->k2tree.produce_proto(&k2tree);
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

  if (!ifstream.good()) {
    std::cerr << "Error while opening file '" << file_path << "'" << std::endl;
    return;
  }

  uint32_t max_sz = read_u32(ifstream);
  uint32_t map_sz = read_u32(ifstream);

  std::vector<char> buf(max_sz, 0);
  for (uint32_t i = 0; i < map_sz; i++) {
    uint32_t curr_sz = read_u32(ifstream);
    ifstream.read(buf.data(), curr_sz);
    proto_msg::K2Tree k2tree;
    k2tree.ParseFromArray(buf.data(), curr_sz);
    predicates_map[k2tree.predicate_index()] =
        std::make_unique<K2TreeWMutex>(K2Tree(k2tree));
  }
  ifstream.close();
}

void PredicatesIndexCache::add_predicate(uint64_t predicate_index) {
  predicates_map[predicate_index] =
      std::make_unique<K2TreeWMutex>(K2Tree(32, 1024));
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
    std::unordered_map<uint64_t, std::unique_ptr<K2TreeWMutex>>
        &&predicates_map)
    : predicates_map(std::move(predicates_map)) {}

PredicatesIndexCacheBuilder::PredicatesIndexCacheBuilder(
    int worker_pool_size, unsigned long max_queue_size)
    : worker_pool(worker_pool_size), max_queue_size(max_queue_size) {}

void PredicatesIndexCacheBuilder::insert_point(uint64_t subject_id,
                                               uint64_t predicate_id,
                                               uint64_t object_id) {
  RDFTriple triple{};
  triple.subject = subject_id;
  triple.predicate = predicate_id;
  triple.object = object_id;

  std::unique_lock<std::mutex> ul(m);
  cv.wait(ul,
          [this]() { return this->insertion_queue.size() < max_queue_size; });
  insertion_queue.push_back(triple, predicate_id);

  auto task = [this, predicate_id]() {
    std::unique_lock<std::mutex> lock(m);
    unsigned long current_predicate = predicate_id;
    auto &k2tree_wmutex = this->get_k2tree_wmutex(current_predicate);
    auto &k2tree = k2tree_wmutex.k2tree;
    k2tree_wmutex.busy = true;
    auto start = std::chrono::high_resolution_clock::now();
    lock.unlock();
    while (!this->insertion_queue.empty_at(current_predicate)) {
      auto next = this->insertion_queue.pop_front(current_predicate);
      k2tree.insert(next.subject, next.object);
    }
    lock.lock();
    k2tree_wmutex.busy = false;
    this->measured_insertion_time +=
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now() - start)
            .count();

    lock.unlock();
    this->cv.notify_all();
  };

  if (!has_predicate(predicate_id)) {
    add_predicate(predicate_id);
    auto &k2tree_wmutex = get_k2tree_wmutex(predicate_id);
    k2tree_wmutex.busy = true;
    worker_pool.add_task(task);
  } else {
    auto &k2tree_wmutex = get_k2tree_wmutex(predicate_id);
    if (!k2tree_wmutex.busy) {
      k2tree_wmutex.busy = true;
      worker_pool.add_task(task);
    }
  }
}

double PredicatesIndexCacheBuilder::get_measured_insertion_time() {
  return measured_insertion_time;
}
void PredicatesIndexCacheBuilder::wait_workers() {
  std::unique_lock<std::mutex> ul(m);
  cv.wait(ul, [this]() { return this->insertion_queue.empty(); });
  worker_pool.stop_all_workers();
  worker_pool.wait_workers();
}

std::unique_ptr<PredicatesIndexCache> PredicatesIndexCacheBuilder::get() {
  return std::make_unique<PredicatesIndexCache>(std::move(predicates_map));
}

K2TreeWMutex &
PredicatesIndexCacheBuilder::get_k2tree_wmutex(uint64_t predicate_index) {
  return *predicates_map[predicate_index];
}

bool PredicatesIndexCacheBuilder::has_predicate(uint64_t predicate_index) {
  return predicates_map.find(predicate_index) != predicates_map.end();
}

void PredicatesIndexCacheBuilder::add_predicate(uint64_t predicate_index) {
  predicates_map[predicate_index] =
      std::make_unique<K2TreeWMutex>(K2Tree(32, 1024));
}