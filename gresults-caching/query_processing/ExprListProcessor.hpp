
//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_EXPR_LIST_PROCESSOR_HPP
#define RDFCACHEK2_EXPR_LIST_PROCESSOR_HPP

#include <memory>
#include <unordered_map>
#include <string>

#include <request_msg.pb.h>

#include "ResultTable.hpp"
#include "VarIndexManager.hpp"
#include "PredicatesCacheManager.hpp"

class ExprListProcessor {
  ResultTable &table;
  const VarIndexManager &vim;
  const std::vector<const proto_msg::ExprNode *> &expr_list;
  const PredicatesCacheManager &cm;

public:
  ExprListProcessor(ResultTable &table, const VarIndexManager &vim,
                    const std::vector<const proto_msg::ExprNode *> &expr_list, const PredicatesCacheManager &cm);
  void execute();
  std::unordered_map<std::string, unsigned long> get_var_pos_mapping();
};

#endif