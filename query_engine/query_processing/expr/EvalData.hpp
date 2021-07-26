
//
// Created by Cristobal Miranda, 2021
//

#ifndef RDFCACHEK2_EVAL_DATA_HPP
#define RDFCACHEK2_EVAL_DATA_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include <TimeControl.hpp>
#include <query_processing/VarBindingQProc.hpp>
#include <request_msg.pb.h>

#include "../VarIndexManager.hpp"
#include "BoundVarsMap.hpp"
#include "PredicatesCacheManager.hpp"
#include "ResultTable.hpp"

struct EvalData {
  VarIndexManager &vim;
  std::shared_ptr<PredicatesCacheManager> cm;
  std::shared_ptr<std::unordered_map<std::string, unsigned long>>
      var_pos_mapping;
  std::shared_ptr<NaiveDynamicStringDictionary> extra_dict;
  TimeControl &time_control;
  std::shared_ptr<VarBindingQProc> var_binding_qproc;
  const std::string &temp_files_dir;
  EvalData(VarIndexManager &vim, std::shared_ptr<PredicatesCacheManager> cm,
           std::shared_ptr<std::unordered_map<std::string, unsigned long>>
               var_pos_mapping,
           std::shared_ptr<NaiveDynamicStringDictionary> extra_dict,
           TimeControl &time_control,
           std::shared_ptr<VarBindingQProc> var_binding_qproc,
           const std::string &temp_files_dir)
      : vim(vim), cm(std::move(cm)),
        var_pos_mapping(std::move(var_pos_mapping)),
        extra_dict(std::move(extra_dict)), time_control(time_control),
        var_binding_qproc(std::move(var_binding_qproc)),
        temp_files_dir(temp_files_dir) {}
};

#endif