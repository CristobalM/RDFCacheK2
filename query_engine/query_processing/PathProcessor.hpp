//
// Created by cristobal on 13-06-21.
//

#ifndef RDFCACHEK2_PATHPROCESSOR_HPP
#define RDFCACHEK2_PATHPROCESSOR_HPP

#include "VarIndexManager.hpp"
#include <PredicatesCacheManager.hpp>
class PathProcessor {
  const PredicatesCacheManager &cm;
  VarIndexManager &vim;
  NaiveDynamicStringDictionary &extra_str_dict;
  const proto_msg::TripleWithPath &triple_with_path;

  std::shared_ptr<ResultTable> result_table;

public:
  PathProcessor(const PredicatesCacheManager &cm, VarIndexManager &vim,
                NaiveDynamicStringDictionary &extra_str_dict,
                const proto_msg::TripleWithPath &triple_with_path);
  std::shared_ptr<ResultTable> execute();
  std::shared_ptr<ResultTable>
  process_path_node(const proto_msg::PathNode &node);
  std::shared_ptr<ResultTable>
  process_link_path(const proto_msg::LinkPath &path);
  std::shared_ptr<ResultTable>
  process_inverse_path(const proto_msg::InversePath &path);
  std::shared_ptr<ResultTable>
  process_sequence_path(const proto_msg::SequencePath &path);
  std::shared_ptr<ResultTable>
  process_alternative_path(const proto_msg::AlternativePath &path);
  std::shared_ptr<ResultTable>
  process_zero_or_more_path(const proto_msg::ZeroOrMorePath &path);
  std::shared_ptr<ResultTable>
  process_one_or_more_path(const proto_msg::OneOrMorePath &path);
  std::shared_ptr<ResultTable>
  process_zero_or_one_path(const proto_msg::ZeroOrMorePath &path);
  std::shared_ptr<ResultTable>
  process_negated_property_set(const proto_msg::NegatedPropertySet &set);
  std::shared_ptr<ResultTable>
  process_fixed_length_parth(const proto_msg::FixedLengthPath &path);
  std::shared_ptr<ResultTable>
  process_multi_length_parth(const proto_msg::MultiLengthPath &path);
  std::shared_ptr<ResultTable>
  process_distinct_path(const proto_msg::DistinctPath &path);
};

#endif // RDFCACHEK2_PATHPROCESSOR_HPP
