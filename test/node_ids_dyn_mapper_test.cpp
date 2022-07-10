//
// Created by cristobal on 19-06-22.
//

#include "CacheContainerImpl.hpp"
#include "I_FileRWHandler.hpp"
#include "cache_test_util.hpp"
#include "mock_structures/TestingTaskProcessor.hpp"
#include "nodeids/NodeIdsManagerFactory.hpp"
#include "server/session/UpdaterSession.hpp"
#include <gtest/gtest.h>
#include <vector>
using namespace k2cache;
// TEST(node_ids_dyn_mapper_test, can_map_big_ids_to_smaller) {
//   auto pcm = basic_pcm();
//   static constexpr auto total_points = 10000UL;
//
//   auto update_config = UpdaterSession::get_initial_update_k2tree_config();
//   auto predicate_id = 343UL;
//   K2TreeMixed k2tree(update_config);
//   K2TreeBulkOp op(k2tree);
//
//   for (auto i = 0UL; i < total_points; i++) {
//     auto j =  i + 1;
//     op.insert(j, j);
//   }
//   auto updates = std::vector<K2TreeUpdates>{
//       {predicate_id, &k2tree, nullptr},
//   };
//   pcm->get_updates_logger().log(updates);
//   ASSERT_TRUE(pcm->get_updates_logger().has_predicate_stored(predicate_id));
//
////  ASSERT_TRUE(
////      pcm->get_predicates_index_cache().has_predicate_active(predicate_id));
//  auto f = pcm->get_predicates_index_cache().fetch_k2tree(predicate_id);
//  ASSERT_TRUE(f.exists());
//  auto &tree = f.get_mutable();
//  ASSERT_EQ(tree.size(), total_points);
//  for (auto i = 0UL; i < total_points; i++) {
//    auto j =   i + 1;
//    ASSERT_TRUE(tree.has(j, j));
//  }
//}

TEST(node_ids_dyn_mapper_test, can_map_big_ids_to_smaller) {
  auto cache_container = mock_cache_container();
  TestingTaskProcessor tp(*cache_container);
  UpdaterSession session(&tp, cache_container.get());
  static constexpr auto total_points = 10000UL;
  auto predicate_id = 342UL;
  for (auto i = 0UL; i < total_points; i++) {
    auto j = i + 1;
    TripleNodeId triple((NodeId(j)), NodeId(predicate_id), NodeId(j));
    session.add_triple(triple);
  }
  session.commit_updates();

  auto &triple_streamer = tp.create_triples_streamer({predicate_id});
  auto &streamer =triple_streamer.start_streaming_matching_triples(TripleNodeId(
      NodeId::create_any(), NodeId((long)predicate_id), NodeId::create_any()));
  auto streamer_data = read_all_from_streamer(streamer,(long) predicate_id);
  std::sort(streamer_data.begin(), streamer_data.end(), [](const TripleNodeId &lhs, const TripleNodeId &rhs){
    return lhs.subject.get_value() < rhs.subject.get_value();
  });
  ASSERT_EQ(total_points, streamer_data.size());
  for(auto i = 0UL; i < total_points; i++){
    auto j = i + 1;
    ASSERT_EQ(j, streamer_data[i].subject.get_value());
    ASSERT_EQ(j, streamer_data[i].object.get_value());
    ASSERT_EQ(predicate_id, streamer_data[i].predicate.get_value());
  }
}
