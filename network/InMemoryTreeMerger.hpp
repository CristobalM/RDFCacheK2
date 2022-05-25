//
// Created by cristobal on 21-05-22.
//

#ifndef RDFCACHEK2_INMEMORYTREEMERGER_HPP
#define RDFCACHEK2_INMEMORYTREEMERGER_HPP

#include <functional>
#include <map>

#include "I_DataMerger.hpp"
#include "K2TreeMixed.hpp"
#include "K2TreeUpdates.hpp"
#include "K2TreeBulkOp.hpp"

class InMemoryTreeMerger : public I_DataMerger {
  std::unique_ptr<K2TreeMixed> current_tree;

public:
  void merge_add_tree(unsigned long predicate_id, K2TreeMixed &k2tree) override;
  void merge_delete_tree(unsigned long predicate_id,
                         K2TreeMixed &k2tree) override;
  void merge_update(std::vector<K2TreeUpdates> &updates) override;

private:
  void merge_op(K2TreeMixed &k2tree,
                const std::function<void(K2TreeBulkOp &, unsigned long,
                                         unsigned long)> &op_fun);
};

#endif // RDFCACHEK2_INMEMORYTREEMERGER_HPP
