
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
#include "BoundVarsMap.hpp"
#include "PredicatesCacheManager.hpp"
#include "ResultTable.hpp"

struct EvalData {
  VarIndexManager &vim;
  const PredicatesCacheManager &cm;
  std::shared_ptr<std::unordered_map<std::string, unsigned long>>
      var_pos_mapping;
  NaiveDynamicStringDictionary &extra_dict;
  EvalData(VarIndexManager &vim, const PredicatesCacheManager &cm,
           std::shared_ptr<std::unordered_map<std::string, unsigned long>>
               var_pos_mapping,
           NaiveDynamicStringDictionary &extra_dict)
      : vim(vim), cm(cm), var_pos_mapping(std::move(var_pos_mapping)),
        extra_dict(extra_dict) {}
};

#endif