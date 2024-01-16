//
// Created by cristobal on 22-12-23.
//

#include "k2tree/K2TreeBulkOp.hpp"
#include "k2tree/K2TreeMixed.hpp"
#include "util_algorithms/fisher_yates.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>

using namespace k2cache;

struct ExperimentResult {
  double insertion_time;
  double deletion_time;
  K2TreeConfig config;
};
static ExperimentResult run_experiment(K2TreeConfig config) {

  K2TreeMixed tree(config);
  K2TreeBulkOp op(tree);

  const int sz = 1000000;

  auto points_a = fisher_yates(sz, 1 << 30);
  auto points_b = fisher_yates(sz, 1 << 30);

  double insertion_time_per_point;
  double deletion_time_per_point;

  {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < sz; i++) {
      op.insert(points_a[i], points_b[i]);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
            .count();
    insertion_time_per_point = duration / sz;
  }
  std::vector<int> permutation;
  permutation.reserve(sz);
  for (int i = 0; i < sz; i++) {
    permutation[i] = i;
  }
  std::random_shuffle(permutation.begin(), permutation.end());

  {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < sz; i++) {
      auto pos = permutation[i];
      op.remove(points_a[pos], points_b[pos]);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
            .count();
    deletion_time_per_point = duration / sz;
  }

  return {insertion_time_per_point, deletion_time_per_point, config};
}

int main(int argc, char **argv) {

  if(argc < 2){
    std::cerr << "Expected output-filename" << std::endl;
    return 1;
  }

  std::string fname = argv[1];

  std::vector<int> node_counts = {64, 128, 256, 512, 1024};
  std::vector<int> cut_depths = {0, 4, 8, 10};
  std::vector<int> tree_depths = {32, 38, 44, 52, 64};
//  std::vector<int> node_counts = {128};
//  std::vector<int> cut_depths = {0};
//  std::vector<int> tree_depths = {32};
  std::vector<ExperimentResult> results;
  for (auto nc : node_counts) {
    for (auto cd : cut_depths) {
      for (auto td : tree_depths) {
        if(td + cd > 64){
          continue;
        }
        K2TreeConfig config{};
        config.cut_depth = cd;
        config.max_node_count = nc;
        config.treedepth = td;
        auto r = run_experiment(config);
        results.push_back(r);
      }
    }
  }

  std::ofstream ofs(fname, std::ios::out);
  ofs << "cut_depth,max_node_count,tree_depth,insertion_speed_ns,deletion_"
         "speed_ns\n";

  for (auto r : results) {
    ofs << r.config.cut_depth << "," << r.config.max_node_count << ","
        << r.config.treedepth << "," << r.insertion_time << ","
        << r.deletion_time << "\n";
  }
}