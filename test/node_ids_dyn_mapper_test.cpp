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

TEST(node_ids_dyn_mapper_test, can_map_big_ids_to_smaller) {
  auto cache_container = mock_cache_container();
  TestingTaskProcessor tp(*cache_container);
  UpdaterSession session(&tp, cache_container.get());
  static constexpr auto total_points = 10000UL;
  const auto base_id = 1L << 33L;
  const auto predicate_id = base_id + 342UL;
  const auto predicate_id2 = 342UL;
  const auto base_id2 = predicate_id + 1;
  auto start = std::chrono::high_resolution_clock::now();
  for (auto i = 0UL; i < total_points; i++) {
    auto j = i + 1 + base_id2;
    TripleNodeId triple((NodeId(j)), NodeId(predicate_id), NodeId(j));
    session.add_triple(triple);
  }
  session.commit_updates();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);
  std::cout << "first took: " << duration.count() << " ms" << std::endl;

  {
    UpdaterSession session2(&tp, cache_container.get());

    start = std::chrono::high_resolution_clock::now();
    for (auto i = 0UL; i < total_points; i++) {
      auto j = i + 1 + base_id2;
      TripleNodeId triple((NodeId(j)), NodeId(predicate_id2), NodeId(j));
      session2.add_triple(triple);
    }
    session2.commit_updates();
    auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start);
    std::cout << "second took: " << duration2.count() << " ms" << std::endl;
    ASSERT_GT(duration, duration2); // the first time should do much more work
                                    // than the second time
  }

  auto &triple_streamer = tp.create_triples_streamer({predicate_id});
  auto &streamer = triple_streamer.start_streaming_matching_triples(
      TripleNodeId(NodeId::create_any(), NodeId((long)predicate_id),
                   NodeId::create_any()));
  auto streamer_data = read_all_from_streamer(streamer, (long)predicate_id);
  std::sort(streamer_data.begin(), streamer_data.end(),
            [](const TripleNodeId &lhs, const TripleNodeId &rhs) {
              return lhs.subject.get_value() < rhs.subject.get_value();
            });
  ASSERT_EQ(total_points, streamer_data.size());
  for (auto i = 0UL; i < total_points; i++) {
    auto j = i + 1 + base_id2;
    ASSERT_EQ(j, streamer_data[i].subject.get_value());
    ASSERT_EQ(j, streamer_data[i].object.get_value());
    ASSERT_EQ(predicate_id, streamer_data[i].predicate.get_value());
  }
}
