
//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_EXPR_LIST_PROCESSOR_HPP
#define RDFCACHEK2_EXPR_LIST_PROCESSOR_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include <request_msg.pb.h>

#include "ExprProcessorPersistentData.hpp"
#include "PredicatesCacheManager.hpp"
#include "ResultTable.hpp"
#include "VarIndexManager.hpp"

class ExprListProcessor {
  ResultTable &table;
  const VarIndexManager &vim;
  const std::vector<const proto_msg::ExprNode *> &expr_list;
  const PredicatesCacheManager &cm;
  const NaiveDynamicStringDictionary &extra_str_dict;

public:
  ExprListProcessor(ResultTable &table, const VarIndexManager &vim,
                    const std::vector<const proto_msg::ExprNode *> &expr_list,
                    const PredicatesCacheManager &cm,
                    const NaiveDynamicStringDictionary &extra_str_dict);
  void execute();
  std::unordered_map<std::string, unsigned long> get_var_pos_mapping();
};

#endif