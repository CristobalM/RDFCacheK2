//
// Created by Cristobal Miranda, 2020
//

#include <fstream>
#include <sstream>

#include <parallel/Worker.hpp>

#include "PredicatesIndexCache.hpp"

#include "predicates_index_cache.pb.h"
#include "serialization_util.hpp"

PredicatesIndexCache::PredicatesIndexCache() {}

bool PredicatesIndexCache::has_predicate(uint64_t predicate_index) {
  return predicates_map.find(predicate_index) != predicates_map.end();
}

K2TreeMixed &PredicatesIndexCache::get_k2tree(uint64_t predicate_index) {
  return *predicates_map[predicate_index];
}

void PredicatesIndexCache::dump_to_file(const std::string &file_path) {
  std::fstream outfs(file_path,
                     std::ios::out | std::ios::trunc | std::ios::binary);
  write_u32(outfs, predicates_map.size());
  for (auto &hmap_item : predicates_map) {
    write_u64(outfs, hmap_item.first);
    hmap_item.second->write_to_ostream(outfs);
  }
}

void PredicatesIndexCache::load_dump_file(const std::string &file_path) {
  std::ifstream ifstream(file_path, std::ios::binary | std::ios::out);

  if (!ifstream.good()) {
    std::cerr << "Error while opening file '" << file_path << "'" << std::endl;
    return;
  }

  uint32_t map_sz = read_u32(ifstream);

  for (uint32_t i = 0; i < map_sz; i++) {
    uint64_t predicate_index = read_u64(ifstream);
    predicates_map[predicate_index] =
        std::make_unique<K2TreeMixed>(K2TreeMixed::read_from_istream(ifstream));
  }
  ifstream.close();
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
    std::unordered_map<uint64_t, std::unique_ptr<K2TreeMixed>> &&predicates_map)
    : predicates_map(std::move(predicates_map)) {}

PredicatesIndexCache::predicates_map_t &
PredicatesIndexCache::get_predicates_map() {
  return predicates_map;
}

PredicatesIndexCacheBuilder::PredicatesIndexCacheBuilder(
    int worker_pool_size, unsigned long max_queue_size)
    : worker_pool_size(worker_pool_size), max_queue_size(max_queue_size) {}

void PredicatesIndexCacheBuilder::insert_point(uint64_t subject_id,
                                               uint64_t predicate_id,
                                               uint64_t object_id) {

  RDFTriple triple{};
  triple.subject = subject_id;
  triple.predicate = predicate_id;
  triple.object = object_id;
  insertion_queue.push_back(triple, predicate_id);

  if (insertion_queue.size() >= max_queue_size) {
    insert_batch();
  }
}

void PredicatesIndexCacheBuilder::insert_batch() {
  WorkerPool worker_pool(worker_pool_size);
  volatile unsigned long remaining_points = insertion_queue.size();
  std::mutex m;
  std::condition_variable cv;

  for (auto it = insertion_queue.get_map().begin();
       it != insertion_queue.get_map().end(); it++) {
    auto predicate_id = it->first;
    if (!has_predicate(predicate_id)) {
      add_predicate(predicate_id);
    }
  }

  for (auto it = insertion_queue.get_map().begin();
       it != insertion_queue.get_map().end(); it++) {
    auto predicate_id = it->first;
    auto &queue = it->second;
    auto &k2tree = *predicates_map[predicate_id];
    worker_pool.add_task([&queue, &k2tree, &remaining_points, &m, &cv]() {
      unsigned long to_discount = queue.size();
      while (!queue.empty()) {
        auto next = queue.front();
        queue.pop_front();
        k2tree.insert(next.subject, next.object);
      }
      {
        std::lock_guard<std::mutex> lg(m);
        remaining_points -= to_discount;
      }
      cv.notify_all();
    });
  }

  std::unique_lock<std::mutex> ul(m);
  cv.wait(ul, [&remaining_points]() { return remaining_points == 0; });

  worker_pool.stop_all_workers();
  worker_pool.wait_workers();
  insertion_queue.clear();
}

double PredicatesIndexCacheBuilder::get_measured_insertion_time() {
  return measured_insertion_time;
}

std::unique_ptr<PredicatesIndexCache> PredicatesIndexCacheBuilder::get() {
  return std::make_unique<PredicatesIndexCache>(std::move(predicates_map));
}

K2TreeMixed &PredicatesIndexCacheBuilder::get_k2tree(uint64_t predicate_index) {
  return *predicates_map[predicate_index];
}

bool PredicatesIndexCacheBuilder::has_predicate(uint64_t predicate_index) {
  return predicates_map.find(predicate_index) != predicates_map.end();
}

void PredicatesIndexCacheBuilder::add_predicate(uint64_t predicate_index) {
  predicates_map[predicate_index] = std::make_unique<K2TreeMixed>(32, 256, 10);
}

void PredicatesIndexCacheBuilder::finish() {
  if (insertion_queue.size() > 0) {
    insert_batch();
  }
}