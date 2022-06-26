//
// Created by cristobal on 19-06-22.
//

#include "I_FileRWHandler.hpp"
#include "cache_test_util.hpp"
#include <gtest/gtest.h>

//TEST(node_ids_dyn_mapper_test, can_map_big_ids_to_smaller){
//  auto pcm = basic_pcm();
//  auto updater = pcm.get_updater();
//  static constexpr auto total_points = 10000UL;
//  static constexpr auto base = (1UL << 32UL);
//
//  for(auto i = 0UL; i < total_points; i++){
//    auto j = base + i + 1;
//    updater->add(j, j, j);
//  }
//  updater->commit();
//  auto q = pcm.get_querier();
//  for(auto i = 0UL; i < total_points; i++){
//    auto j = base + i + 1;
//    ASSERT_TRUE(q->has(j, j, j));
//  }
//}
