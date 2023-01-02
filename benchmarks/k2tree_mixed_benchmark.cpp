#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

#include "k2tree/K2TreeBulkOp.hpp"
#include "k2tree/K2TreeMixed.hpp"

#include "util_algorithms/fisher_yates.hpp"
using namespace k2cache;

std::mt19937 rng(123321);

struct BenchmarkResult {
  int treedepth;
  int node_count;
  uint64_t points_count;
  uint64_t size_bytes;
  uint64_t bytes_ptrs;
  uint64_t bytes_topologies;
  uint64_t total_microseconds_inserting;
  uint64_t inserted_points;
  uint64_t serialized_size_bytes;
};

BenchmarkResult space_benchmark_random_insertion_by_depth_and_node_count(
    TREE_DEPTH_T treedepth, MAX_NODE_COUNT_T node_count,
    uint64_t points_count, std::vector<uint64_t> &cols,
    std::vector<uint64_t> &rows);

int main(void) {

  uint64_t points_count = 1 << 25;
  uint64_t treedepth = 32;
  uint64_t side = 1L << treedepth;
  uint64_t side_count =
      std::min((uint64_t)std::sqrt(points_count), side);

  auto random_seq_1 = fisher_yates(side_count, side);
  auto random_seq_2 = fisher_yates(side_count, side);
  std::vector<BenchmarkResult> results;
  std::cout << "seq 1 sz: " << random_seq_1.size()
            << ", seq 2 sz: " << random_seq_2.size()
            << ", side_count: " << side_count
            << ", points_count: " << points_count << std::endl;
  std::cout << "started experiments" << std::endl;
  for (int node_count_base = 6; node_count_base <= 10; node_count_base++) {

    results.push_back(space_benchmark_random_insertion_by_depth_and_node_count(
        treedepth, 1 << node_count_base, points_count, random_seq_1,
        random_seq_2));
  }

  std::cout << "Tree depth,Node count,Points count,Total Bytes,Bytes "
               "Ptrs+Preorders,Bytes Topologies,Total Time(Microsecs), Avg "
               "Time(Microsecs), Serialized Size(Bytes)"
            << std::endl;

  for (auto bm : results) {
    std::cout << bm.treedepth << "," << bm.node_count << ","
              << bm.inserted_points << "," << bm.size_bytes << ","
              << bm.bytes_ptrs << "," << bm.bytes_topologies << ","
              << bm.total_microseconds_inserting << ","
              << (float)bm.total_microseconds_inserting /
                     (float)bm.inserted_points
              << "," << bm.serialized_size_bytes << std::endl;
  }

  return 0;
}

BenchmarkResult space_benchmark_random_insertion_by_depth_and_node_count(
    TREE_DEPTH_T treedepth, MAX_NODE_COUNT_T node_count,
    uint64_t points_count, std::vector<uint64_t> &cols,
    std::vector<uint64_t> &rows) {

  K2TreeMixed k2tree(treedepth, node_count, 8);

  auto start = std::chrono::high_resolution_clock::now();
  uint64_t inserted_points = 0;
  K2TreeBulkOp bulk_op(k2tree);
  for (size_t i = 0; i < cols.size(); i++) {

    for (size_t j = 0; j < rows.size(); j++) {
      bulk_op.insert(cols[i], rows[j]);
      inserted_points++;
      if (inserted_points >= points_count)
        break;
    }
    if (inserted_points >= points_count)
      break;
  }
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

  struct k2tree_measurement measurements = k2tree.measure_in_memory_size();

  std::stringstream ss;

  k2tree.write_to_ostream(ss);

  ss.seekp(0, std::ios::end);
  uint64_t serialized_size = ss.tellp();

  return {treedepth,
          node_count,
          points_count,
          measurements.total_bytes,
          measurements.total_blocks *
              (sizeof(uint32_t) + sizeof(struct block *)),
          measurements.bytes_topology,
          (uint64_t)duration.count(),
          inserted_points,
          serialized_size};
}