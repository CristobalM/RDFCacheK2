//
// Created by cristobal on 16-06-22.
//

#include "K2TreeMixed.hpp"
#include <iostream>
#include <random>

static long random_generate(long start, long end) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<long> distr(start, end);
  return distr(gen);
}
//
static void fully_random() {
  K2TreeConfig config{};
  config.cut_depth = 0;
  config.treedepth = 32;
  config.max_node_count = 1024;

  int total_counts = 100;

  std::cout << "total_bytes"
            << ","
            << "points"
            << ","
            << "total_bytes/points"
            << ","
            << "total_blocks"
            << ","
            << "bytes_topology"
            << "\n";
  for (int count = 0; count < total_counts; count++) {
    K2TreeMixed tree(config);
    long points = 100'000L;
    for (long i = 0; i < points; i++) {
      for (;;) {
        long col = random_generate(0, (1L << 32L) - 1L);
        long row = random_generate(0, (1L << 32L) - 1L);
        if (tree.has(col, row))
          continue;
        tree.insert(col, row);
        break;
      }
    }

    auto stats = tree.k2tree_stats();
    std::cout << stats.total_bytes << "," << points << ","
              << double(stats.total_bytes) / double(points) << ","
              << stats.total_blocks << "," << stats.bytes_topology << std::endl;
  }
}
//
// static void random_4point_blocks() {
//  K2TreeConfig config{};
//  config.cut_depth = 0;
//  config.treedepth = 32;
//  config.max_node_count = 1024;
//
//  int total_counts = 100;
//
//  std::cout << "total_bytes" << ","
//            << "points" << ","
//            << "total_bytes/points" << ","
//            << "total_blocks" << ","
//            << "bytes_topology" << "\n";
//  for (int count = 0; count < total_counts; count++) {
//    K2TreeMixed tree(config);
//    long points = 100'000L;
//    for (long i = 0; i < points; i+= 4) {
//      for (;;) {
//        long col = random_generate(0, (1L << 32L) - 2L);
//        long row = random_generate(0, (1L << 32L) - 2L);
//        if (tree.has(col, row) || tree.has(col+1, row+1) || tree.has(col+1,
//        row) || tree.has(col, row+1))
//          continue;
//        tree.insert(col, row);
//        tree.insert(col+1, row+1);
//        tree.insert(col+1, row);
//        tree.insert(col, row+1);
//        break;
//      }
//    }
//
//    auto stats = tree.k2tree_stats();
//    std::cout << stats.total_bytes << ","
//              << points << ","
//              << double(stats.total_bytes)/double(points) << ","
//              << stats.total_blocks << ","
//              << stats.bytes_topology << std::endl;
//  }
//}
//
// static void random_2point_diag() {
//  K2TreeConfig config{};
//  config.cut_depth = 0;
//  config.treedepth = 32;
//  config.max_node_count = 1024;
//
//  int total_counts = 100;
//
//  std::cout << "total_bytes" << ","
//            << "points" << ","
//            << "total_bytes/points" << ","
//            << "total_blocks" << ","
//            << "bytes_topology" << "\n";
//  for (int count = 0; count < total_counts; count++) {
//    K2TreeMixed tree(config);
//    long points = 100'000L;
//    for (long i = 0; i < points; i+= 2) {
//      for (;;) {
//        long col = random_generate(0, (1L << 32L) - 2L);
//        long row = random_generate(0, (1L << 32L) - 2L);
//        if (tree.has(col, row) || tree.has(col+1, row+1))
//          continue;
//        tree.insert(col, row);
//        tree.insert(col+1, row+1);
//        break;
//      }
//    }
//
//    auto stats = tree.k2tree_stats();
//    std::cout << stats.total_bytes << ","
//              << points << ","
//              << double(stats.total_bytes)/double(points) << ","
//              << stats.total_blocks << ","
//              << stats.bytes_topology << std::endl;
//  }
//}

int main() { fully_random(); }