//
// Created by cristobal on 9/7/21.
//

#ifndef RDFCACHEK2_DATAMERGERMOCK_HPP
#define RDFCACHEK2_DATAMERGERMOCK_HPP

#include <memory>
#include <unordered_map>
#include <vector>

#include <I_DataMerger.hpp>
#include <K2TreeMixed.hpp>
#include <NaiveDynamicStringDictionary.hpp>

struct DataMergerMock : public I_DataMerger {

  K2TreeConfig config;
  explicit DataMergerMock(K2TreeConfig config);

  NaiveDynamicStringDictionary main_dict;

  std::unordered_map<unsigned long, std::unique_ptr<K2TreeMixed>> trees;

  void merge_with_extra_dict(
      NaiveDynamicStringDictionary &input_extra_dict) override;

  void merge_add_tree(unsigned long predicate_id, K2TreeMixed &k2tree) override;
  void merge_delete_tree(unsigned long predicate_id,
                         K2TreeMixed &k2tree) override;

  void drop();
  void merge_update(std::vector<K2TreeUpdates> &updates) override;
};

#endif // RDFCACHEK2_DATAMERGERMOCK_HPP
