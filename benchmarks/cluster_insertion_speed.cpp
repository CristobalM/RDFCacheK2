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

enum class OpType {
  INSERT,
  DELETE
};

template<OpType T>
static void op_cluster(K2TreeBulkOp &op, uint64_t col, uint64_t row,
                           uint64_t radius, uint64_t limit, int &count) {
  auto min_col = col - radius;
  min_col = std::max(min_col, (uint64_t)0);
  auto max_col = col + radius;
  max_col = std::min(max_col, limit);

  auto min_row = row - radius;
  min_row = std::max(min_row, (uint64_t)0);
  auto max_row = row + radius;
  max_row = std::min(max_row, limit);

  for (uint64_t c = min_col; c < max_col; c++) {
    for (uint64_t r = min_row; r < max_row; r++) {
      if constexpr (T == OpType::INSERT){
         op.insert(c, r);
      }
      else{
        op.remove(c, r);
      }
      count++;
    }
  }
}

static ExperimentResult run_experiment(K2TreeConfig config, int radius) {

  K2TreeMixed tree(config);
  K2TreeBulkOp op(tree);

  int sz = 1000000;

  int radius_points = radius * radius;

  if (radius_points <= 0){
    std::cerr << " invalid radius points: " << radius_points << std::endl;
    return {};
  }

  sz /= (4 * radius_points);

  auto points_a = fisher_yates(sz, 1 << 30);
  auto points_b = fisher_yates(sz, 1 << 30);

  double insertion_time_per_point;
  double deletion_time_per_point;

  uint64_t limit = (uint64_t )1 << (uint64_t)config.treedepth;

  int count = 0;
  {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < sz; i++) {
      op_cluster<OpType::INSERT>(op, points_a[i], points_b[i], (uint64_t )radius, limit, count);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
            .count();
    insertion_time_per_point = (double)duration / (double)(sz*radius_points);
  }

  std::cout << "total points added " << count << std::endl;
  std::vector<int> permutation;
  permutation.reserve(sz);
  for (int i = 0; i < sz; i++) {
    permutation[i] = i;
  }
  std::random_shuffle(permutation.begin(), permutation.end());

  count = 0;
  {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < sz; i++) {
      auto pos = permutation[i];
      op_cluster<OpType::DELETE>(op, points_a[pos], points_b[pos], (uint64_t )radius, limit, count);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
            .count();
    deletion_time_per_point = (double)duration / (double)(sz*radius_points);
  }

  std::cout << "total points deleted " << count << std::endl;


  return {insertion_time_per_point, deletion_time_per_point, config};
}

int main(int argc, char **argv) {
  if(argc < 3){
    std::cerr << "Expected: <radius> <output-filename>" << std::endl;
    return  1;
  }

  auto radius = std::stoi(argv[1]);
  std::string fname = argv[2];


  std::vector<int> node_counts = {64, 128, 256, 512, 1024};
  std::vector<int> cut_depths = {0, 4, 8, 10};
  std::vector<int> tree_depths = {32, 38, 44, 52, 64};
  //  std::vector<int> node_counts = {128};
  //  std::vector<int> cut_depths = {0};
  //  std::vector<int> tree_depths = {32};
//  std::vector<ExperimentResult> results;

  std::ofstream ofs(fname, std::ios::out);
  ofs << "cut_depth,max_node_count,tree_depth,insertion_speed_ns,deletion_"
         "speed_ns\n";

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
        auto r = run_experiment(config, radius);
//        results.push_back(r);
        ofs << r.config.cut_depth << "," << r.config.max_node_count << ","
            << r.config.treedepth << "," << (uint64_t)r.insertion_time << ","
            << (uint64_t)r.deletion_time << "\n";
        ofs.flush();
      }
    }
  }



//  for (auto r : results) {
//    ofs << r.config.cut_depth << "," << r.config.max_node_count << ","
//        << r.config.treedepth << "," << r.insertion_time << ","
//        << r.deletion_time << "\n";
//  }
}