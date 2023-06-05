//
// Created by cristobal on 05-06-23.
//
#include <TimeControl.hpp>
#include <chrono>
#include <gtest/gtest.h>

#include <memory>

#include "bgp_test_util.hpp"
#include "cache_test_util.hpp"
#include "k2tree/K2TreeMixed.hpp"
#include "query_processing/BGPProcessor.hpp"

using namespace k2cache;
using namespace std::chrono_literals;
TEST(bgp_processor_test, right_permutation) {
  // boilerplate
  DataHolders dh{};
  auto cc = mock_cache_container(dh, false);
  VarIndexManager vim;
  TimeControl tc(1000, 30000ms);
  // end boilerplate
  BGPMessage msg = create_simple_bgp_message(4);

  std::reverse(msg.patterns.begin(), msg.patterns.end());

  auto bgp_proc = std::make_unique<BGPProcessor>(
      msg, cc->get_pcm(), cc->get_nodes_ids_manager(), vim, tc);
  bgp_proc->execute_it();
  auto perm_vec = bgp_proc->get_permutation_vec();
  for(auto value: perm_vec){
    std::cout << value << " ";
  }
  std::cout << std::endl;

  ASSERT_EQ(perm_vec.size(), msg.var_names.size());
  ASSERT_EQ(perm_vec[0], 3);
  ASSERT_EQ(perm_vec[1], 2);
  ASSERT_EQ(perm_vec[2], 1);
  ASSERT_EQ(perm_vec[3], 0);
}
TEST(bgp_processor_test, right_permutation_2) {
  // boilerplate
  DataHolders dh{};
  auto cc = mock_cache_container(dh, false);
  VarIndexManager vim;
  TimeControl tc(1000, 30000ms);
  // end boilerplate
  auto msg = create_simple_bgp_message(4);
  std::reverse(msg.patterns.begin(), msg.patterns.end());

  std::swap(msg.var_names[0], msg.var_names[3]);

  auto bgp_proc = std::make_unique<BGPProcessor>(
      msg, cc->get_pcm(), cc->get_nodes_ids_manager(), vim, tc);
  bgp_proc->execute_it();
  auto perm_vec = bgp_proc->get_permutation_vec();
  for(auto value: perm_vec){
    std::cout << value << " ";
  }
  std::cout << std::endl;

  ASSERT_EQ(perm_vec.size(), msg.var_names.size());
  ASSERT_EQ(perm_vec[0], 0);
  ASSERT_EQ(perm_vec[1], 2);
  ASSERT_EQ(perm_vec[2], 1);
  ASSERT_EQ(perm_vec[3], 3);
}
