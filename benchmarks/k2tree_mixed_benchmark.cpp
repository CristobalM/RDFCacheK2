#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <K2TreeMixed.hpp>

#include "fisher_yates.hpp"

std::mt19937 rng(123321);

struct BenchmarkResult {
  int treedepth;
  int node_count;
  unsigned long points_count;
  unsigned long size_bytes;
  unsigned long bytes_ptrs;
  unsigned long bytes_topologies;
  unsigned long total_microseconds_inserting;
  unsigned long inserted_points;
  unsigned long serialized_size_bytes;
};

BenchmarkResult space_benchmark_random_insertion_by_depth_and_node_count(
    TREE_DEPTH_T treedepth, MAX_NODE_COUNT_T node_count,
    unsigned long points_count, std::vector<unsigned long> &cols,
    std::vector<unsigned long> &rows);

int main(void) {

  unsigned long points_count = 1 << 25;
  unsigned long treedepth = 32;
  unsigned long side = 1L << treedepth;
  unsigned long side_count =
      std::min((unsigned long)std::sqrt(points_count), side);

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
    unsigned long points_count, std::vector<unsigned long> &cols,
    std::vector<unsigned long> &rows) {

  K2TreeMixed k2tree(treedepth, node_count, 8);

  auto start = std::chrono::high_resolution_clock::now();
  unsigned long inserted_points = 0;
  for (size_t i = 0; i < cols.size(); i++) {

    for (size_t j = 0; j < rows.size(); j++) {
      k2tree.insert(cols[i], rows[j]);
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
  unsigned long serialized_size = ss.tellp();

  return {treedepth,
          node_count,
          points_count,
          measurements.total_bytes,
          measurements.total_blocks *
              (sizeof(uint32_t) + sizeof(struct block *)),
          measurements.bytes_topology,
          (unsigned long)duration.count(),
          inserted_points,
          serialized_size};
}