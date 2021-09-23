//
// Created by cristobal on 9/6/21.
//

#include <K2TreeBulkOp.hpp>
#include <UpdatesLogger.hpp>
#include <gtest/gtest.h>

#include "mock_structures/DataMergerMock.hpp"
#include "mock_structures/FHMock.hpp"
#include "mock_structures/StringIStream.hpp"

TEST(update_log_test, test_1) {
  K2TreeConfig config;
  config.treedepth = 32;
  config.cut_depth = 10;
  config.max_node_count = 256;
  DataMergerMock data_merger(config);
  std::string data;
  std::string data_offsets;
  std::string metadata;
  FHMock fh(data);
  FHMock fh_offsets(data_offsets);
  FHMock fh_metadata(data_offsets);
  UpdatesLogger updates_logger(data_merger, fh, fh_offsets, fh_metadata);

  int predicate_id_1 = 123;

  K2TreeMixed k2tree(config);
  K2TreeBulkOp op(k2tree);
  int size_tree = 10000;
  for (int i = 0; i < size_tree; i++) {
    op.insert(i + 1, i + 1);
  }
  K2TreeUpdates tree_update_1(predicate_id_1, &k2tree, nullptr);
  std::vector<K2TreeUpdates> tree_updates = {tree_update_1};
  updates_logger.log(tree_updates);

  ASSERT_TRUE(updates_logger.has_predicate_stored(predicate_id_1));
  ASSERT_EQ(k2tree.size(), size_tree);
  data_merger.drop();
  ASSERT_EQ(data_merger.trees.size(), 0);
  updates_logger.recover_all();
  ASSERT_EQ(data_merger.trees.size(), 1);

  auto &dm_tree = *data_merger.trees[predicate_id_1];

  ASSERT_EQ(dm_tree.size(), k2tree.size());
}
