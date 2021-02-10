#include <parallel/Worker.hpp>

#include "PredicatesIndexCacheBuilder.hpp"


PredicatesIndexCacheBuilder::PredicatesIndexCacheBuilder(
    int worker_pool_size, unsigned long max_queue_size, K2TreeConfig config)
    : worker_pool_size(worker_pool_size), max_queue_size(max_queue_size), config(std::move(config)) {}

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
  predicates_map[predicate_index] = std::make_unique<K2TreeMixed>(config.treedepth, config.max_node_count, config.cut_depth);
}

void PredicatesIndexCacheBuilder::finish() {
  if (insertion_queue.size() > 0) {
    insert_batch();
  }
}
