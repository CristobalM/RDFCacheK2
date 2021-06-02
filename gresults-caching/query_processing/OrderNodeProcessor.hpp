//
// Created by cristobal on 5/31/21.
//

#ifndef RDFCACHEK2_ORDERNODEPROCESSOR_HPP
#define RDFCACHEK2_ORDERNODEPROCESSOR_HPP

#include "VarIndexManager.hpp"
#include <PredicatesCacheManager.hpp>
#include <ResultTable.hpp>
#include <memory>
#include <sparql_tree.pb.h>
class OrderNodeProcessor {
  std::shared_ptr<ResultTable> current_table;
  const proto_msg::OrderNode &node;
  const PredicatesCacheManager &cm;
  VarIndexManager &vim;

public:
  OrderNodeProcessor(std::shared_ptr<ResultTable> input_table,
                     const proto_msg::OrderNode &node,
                     const PredicatesCacheManager &cm, VarIndexManager &vim);
  std::shared_ptr<ResultTable> execute();
  std::vector<int> get_permutation();
  static std::string extract_cmp_str_from_resource(const RDFResource &resource);
};

#endif // RDFCACHEK2_ORDERNODEPROCESSOR_HPP
