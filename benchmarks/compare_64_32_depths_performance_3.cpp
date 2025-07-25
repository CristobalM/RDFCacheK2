//
// Created by cristobal on 10/11/21.
//
#include <chrono>
#include <iostream>

#include "k2tree/K2TreeMixed.hpp"
using namespace k2cache;
int main(int, char **) {
  K2TreeConfig config1{};
  config1.cut_depth = 10;
  config1.treedepth = 32;
  config1.max_node_count = 256;
  K2TreeMixed tree1(config1);
  K2TreeConfig config2{};
  config2.cut_depth = 10;
  config2.treedepth = 64;
  config2.max_node_count = 256;
  K2TreeMixed tree2(config2);

  static constexpr uint64_t TEN_MILL = 10'000'000;

  std::cout << "inserting to tree1 " << std::endl;
  auto start = std::chrono::high_resolution_clock::now();
  for (size_t i = 0; i < TEN_MILL; i++) {
    tree1.insert(i, i);
  }
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

  std::cout << "took " << duration.count() << " ms" << std::endl;
  std::cout << "inserting to tree2 " << std::endl;
  start = std::chrono::high_resolution_clock::now();
  for (size_t i = 0; i < TEN_MILL; i++) {
    tree2.insert(i, i);
  }
  stop = std::chrono::high_resolution_clock::now();
  duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
  std::cout << "took " << duration.count() << " ms" << std::endl;

  std::cout << "scanning... " << std::endl;
  start = std::chrono::high_resolution_clock::now();

  tree1.scan_points(
      [](uint64_t col, uint64_t, void *) { (void)(col); }, nullptr);

  stop = std::chrono::high_resolution_clock::now();
  duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

  std::cout << "full scan 1 took " << duration.count() << " ms" << std::endl;

  start = std::chrono::high_resolution_clock::now();

  tree2.scan_points(
      [](uint64_t col, uint64_t, void *) { (void)(col); }, nullptr);

  stop = std::chrono::high_resolution_clock::now();
  duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

  std::cout << "full scan 2 took " << duration.count() << " ms" << std::endl;
}