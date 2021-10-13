//
// Created by cristobal on 10/11/21.
//
#include <chrono>
#include <iostream>

#include <K2TreeMixed.hpp>
int main(int, char **){
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

  static constexpr unsigned long TEN_MILL = 10'000'000;
  static constexpr unsigned long THOUSAND = 1'000;

  std::cout << "inserting to tree1 " << std::endl;
  for(size_t i = TEN_MILL; i < TEN_MILL + THOUSAND; i++){
    for(size_t j = TEN_MILL; j < TEN_MILL + THOUSAND; j++)
    tree1.insert(i, j);
  }

  std::cout << "inserting to tree2 " << std::endl;
  for(size_t i = TEN_MILL; i < TEN_MILL + THOUSAND; i++){
    for(size_t j = TEN_MILL; j < TEN_MILL + THOUSAND; j++)
    tree2.insert(i, j);
  }

  std::cout << "scanning... " << std::endl;
  auto scanner1 = tree1.create_full_scanner();

  int count1  = 0;
  auto start = std::chrono::high_resolution_clock::now();
  while(scanner1->has_next()){
    scanner1->next();
    count1++;
  }
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

  std::cout << "full scan for scanner1 took " << duration.count() << " ms, elements counted: " << count1 << std::endl;

  int count2  = 0;

  auto scanner2 = tree2.create_full_scanner();

  start = std::chrono::high_resolution_clock::now();
  while(scanner2->has_next()){
    scanner2->next();
    count2++;
  }
  stop = std::chrono::high_resolution_clock::now();
  duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

  std::cout << "full scan for scanner2 took " << duration.count() << " ms, elements counted: " << count2 << std::endl;



}