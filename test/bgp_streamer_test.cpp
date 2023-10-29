//
// Created by cristobal on 05-06-23.
//
#include <TimeControl.hpp>
#include <chrono>
#include <gtest/gtest.h>

#include <memory>

#include "k2tree/K2TreeBulkOp.hpp"
#include "k2tree/K2TreeMixed.hpp"
#include "query_processing/BGPProcessor.hpp"
#include "query_processing/iterators/bgpops/OneVarCPBGPOp.hpp"
#include "query_processing/iterators/bgpops/OneVarIntersectBGPOp.hpp"

#include "bgp_test_util.hpp"
#include "cache_test_util.hpp"
#include "streaming/BGPStreamer.hpp"

using namespace k2cache;
using namespace std::chrono_literals;

TEST(bgp_streamer_test, right_permutation) {
  // boilerplate
  auto cc = create_empty_cache_container();
  VarIndexManager vim;
  TimeControl tc(1000, 30000ms);
  // end boilerplate

  const auto msg_sz = 4;

  const uint64_t start_real_id = 4000;
  uint64_t curr_real_id = start_real_id;

  auto &pred_cache = cc->get_pcm().get_predicates_index_cache();

  auto &nim = cc->get_nodes_ids_manager();
  auto next_id_get = [&](){
    return nim.get_id_or_create(++curr_real_id);
  };
  next_id_get();
  std::vector<uint64_t> real_preds;
  std::vector<uint64_t> cache_preds;
  std::vector<K2TreeBulkOp> ops;

  auto insert_message =
      [](uint64_t subject, uint64_t predicate, uint64_t object,
         uint64_t real_subject, uint64_t real_pred, uint64_t real_obj){
    std::cout << " inserting cache_ids (" << subject << ", " << predicate << ", " << object << ")"
                  << " -> real (" << real_subject << ", " << real_pred << ", " << real_obj << ")"
                  << std::endl;
  };

  for(auto i = 0; i < msg_sz; ++i) {
    auto pred_id = next_id_get();
    real_preds.push_back(curr_real_id);
    cache_preds.push_back(pred_id);
    pred_cache.add_predicate(pred_id);
    auto fetched = pred_cache.fetch_k2tree(pred_id);
    ASSERT_TRUE(fetched.exists());
    auto &k2tree = fetched.get_mutable();
    K2TreeBulkOp op(k2tree);
    ops.push_back(std::move(op));
  }
  //

  auto subj_cache_1 = next_id_get();
  auto subj_real_1 = curr_real_id;
  auto obj_cache_1 = next_id_get();
  auto obj_real_1 = curr_real_id;

  ops[0].insert(subj_cache_1, obj_cache_1);
  insert_message(subj_cache_1, cache_preds[0], obj_cache_1,
                 subj_real_1,real_preds[0], obj_real_1);

  auto subj_cache_2 = obj_cache_1;
  auto subj_real_2 = obj_real_1;
  auto obj_cache_2 = next_id_get();
  auto obj_real_2 = curr_real_id;
  ops[1].insert(subj_cache_2, obj_cache_2);
  insert_message(subj_cache_2, cache_preds[1], obj_cache_2,
                 subj_real_2, real_preds[1], obj_real_2);

  auto subj_cache_3 = obj_cache_2;
  auto subj_real_3 = obj_real_2;
  auto obj_cache_3 = next_id_get();
  auto obj_real_3 = curr_real_id;
  ops[2].insert(subj_cache_3, obj_cache_3);
  insert_message(subj_cache_3, cache_preds[2], obj_cache_3,
                 subj_real_3, real_preds[2], obj_real_3);
  auto subj_cache_4 = obj_cache_3;
  auto subj_real_4 = obj_real_3;
  auto obj_cache_4 = next_id_get();
  auto obj_real_4 = curr_real_id;
  ops[3].insert(subj_cache_4, obj_cache_4);
  insert_message(subj_cache_4, cache_preds[3], obj_cache_4,
                 subj_real_4, real_preds[3], obj_real_4);

  auto msg = create_simple_bgp_message(msg_sz);

  auto print_bgp_node = [](BGPNode &n){
    if(n.is_concrete){
      std::cout << n.real_node_id;
      return;
    }
    std::cout << n.var_name;
  };

  auto print_bgp_msg = [&](BGPMessage &msg){
    std::cout << "--- |";
    for(auto& vn: msg.var_names){
      std::cout << vn << ", ";
    }
    std::cout << "\n===================" << std::endl;
    for(auto& p: msg.patterns){
      std::cout << "(";
      print_bgp_node(p.subject);
      std::cout << ", ";
      print_bgp_node(p.predicate);
      std::cout << ", ";
      print_bgp_node(p.object);
      std::cout << ")\n";
    }
  };

  // This makes a concrete result
  msg.patterns[0].subject.real_node_id = subj_real_1;
  msg.patterns[0].subject.cache_node_id = subj_cache_1;
  msg.patterns[0].subject.is_concrete = true;
  msg.patterns[0].predicate.real_node_id = real_preds[0];
  msg.patterns[0].predicate.cache_node_id = cache_preds[0];
  msg.patterns[0].object.real_node_id = obj_real_1;
  msg.patterns[0].object.cache_node_id = obj_cache_1;

  msg.patterns[1].subject.real_node_id = subj_real_2;
  msg.patterns[1].subject.cache_node_id = subj_cache_2;
  msg.patterns[1].predicate.real_node_id = real_preds[1];
  msg.patterns[1].predicate.cache_node_id = cache_preds[1];
  msg.patterns[1].object.real_node_id = obj_real_2;
  msg.patterns[1].object.cache_node_id = obj_cache_2;

  msg.patterns[2].subject.real_node_id = subj_real_3;
  msg.patterns[2].subject.cache_node_id = subj_cache_3;
  msg.patterns[2].predicate.real_node_id = real_preds[2];
  msg.patterns[2].predicate.cache_node_id = cache_preds[2];
  msg.patterns[2].object.real_node_id = obj_real_3;
  msg.patterns[2].object.cache_node_id = obj_cache_3;

  msg.patterns[3].subject.real_node_id = subj_real_4;
  msg.patterns[3].subject.cache_node_id = subj_cache_4;
  msg.patterns[3].predicate.real_node_id = real_preds[3];
  msg.patterns[3].predicate.cache_node_id = cache_preds[3];
  msg.patterns[3].object.real_node_id = obj_real_4;
  msg.patterns[3].object.cache_node_id = obj_cache_4;

  // Now to make it a variable one
  msg.patterns[0].object.is_concrete = false;
  msg.patterns[0].object.var_name = "?x";
  msg.patterns[1].subject.is_concrete = false;
  msg.patterns[1].subject.var_name = "?x";

  msg.patterns[1].object.is_concrete = false;
  msg.patterns[1].object.var_name = "?y";
  msg.patterns[2].subject.is_concrete = false;
  msg.patterns[2].subject.var_name = "?y";

  msg.patterns[2].object.is_concrete = false;
  msg.patterns[2].object.var_name = "?z";
  msg.patterns[3].subject.is_concrete = false;
  msg.patterns[3].subject.var_name = "?z";

  msg.patterns[3].object.is_concrete = false;
  msg.patterns[3].object.var_name = "?w";

  msg.var_names = { "?x", "?y", "?z", "?w" };

  print_bgp_msg(msg);

  auto streamer = std::make_unique<BGPStreamer>(0, msg, *cc, true);
  auto next_streamer_msg = streamer->get_next_message();
  const auto &join_response = next_streamer_msg.bgp_join_response();
//  ASSERT_EQ(join_response.var_names_size(), msg.var_names.size());

  ASSERT_EQ(join_response.bgp_response_row_size(), 1);
  const auto &response_row = join_response.bgp_response_row(0);
  ASSERT_EQ(response_row.bgp_response_row_values_size(), msg.var_names.size());
  ASSERT_EQ(response_row.bgp_response_row_values(0), obj_real_1);
  ASSERT_EQ(response_row.bgp_response_row_values(0), subj_real_2);

  ASSERT_EQ(response_row.bgp_response_row_values(1), obj_real_2);
  ASSERT_EQ(response_row.bgp_response_row_values(1), subj_real_3);

  ASSERT_EQ(response_row.bgp_response_row_values(2), obj_real_3);
  ASSERT_EQ(response_row.bgp_response_row_values(2), subj_real_4);

  ASSERT_EQ(response_row.bgp_response_row_values(3), obj_real_4);

}
