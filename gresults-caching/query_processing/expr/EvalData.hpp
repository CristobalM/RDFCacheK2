
//
// Created by Cristobal Miranda, 2021
//

#ifndef RDFCACHEK2_EVAL_DATA_HPP
#define RDFCACHEK2_EVAL_DATA_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include <request_msg.pb.h>

#include "../VarIndexManager.hpp"
#include "PredicatesCacheManager.hpp"
#include "ResultTable.hpp"

struct EvalData {
  const ResultTable &table;
  const VarIndexManager &vim;
  const PredicatesCacheManager &cm;
  const std::unordered_map<std::string, unsigned long> &var_pos_mapping;
  EvalData(
      const ResultTable &table, const VarIndexManager &vim,
      const PredicatesCacheManager &cm,
      const std::unordered_map<std::string, unsigned long> &var_pos_mapping)
      : table(table), vim(vim), cm(cm), var_pos_mapping(var_pos_mapping) {}
};

#endif