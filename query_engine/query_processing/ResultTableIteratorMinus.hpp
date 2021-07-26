//
// Created by cristobal on 25-07-21.
//

#ifndef RDFCACHEK2_RESULTTABLEITERATORMINUS_HPP
#define RDFCACHEK2_RESULTTABLEITERATORMINUS_HPP

#include "ResultTableIterator.hpp"
#include "VarBindingQProc.hpp"
#include "VarIndexManager.hpp"
#include <NaiveDynamicStringDictionary.hpp>
#include <PredicatesCacheManager.hpp>
#include <sparql_tree.pb.h>
class ResultTableIteratorMinus : public ResultTableIterator {
  std::shared_ptr<ResultTableIterator> left_it;
  proto_msg::SparqlNode right_node;
  std::shared_ptr<VarBindingQProc> var_binding_qproc;
  bool next_available;
  std::vector<unsigned long> next_result;

  std::shared_ptr<ResultTableIterator> current_right_it;

  std::shared_ptr<PredicatesCacheManager> cache_manager;
  std::shared_ptr<VarIndexManager> vim;
  std::shared_ptr<NaiveDynamicStringDictionary> extra_str_dict;

  std::shared_ptr<VarBindingQProc> current_var_binding_qproc;

  const std::string &temp_files_dir;

public:
  ResultTableIteratorMinus(
      std::shared_ptr<ResultTableIterator> left_it,
      proto_msg::SparqlNode right_node,
      std::shared_ptr<VarBindingQProc> var_binding_qproc,
      TimeControl &time_control,
      std::shared_ptr<PredicatesCacheManager> cache_manager,
      std::shared_ptr<VarIndexManager> vim,
      std::shared_ptr<NaiveDynamicStringDictionary> extra_str_dict,
      const std::string &temp_files_dir);
  bool has_next() override;
  std::vector<unsigned long> next() override;
  std::vector<unsigned long> &get_headers() override;
  void reset_iterator() override;
  std::vector<unsigned long> next_concrete();
  bool create_var_binding_qproc_if_needed(
      std::vector<unsigned long> &current_left_row);
};

#endif // RDFCACHEK2_RESULTTABLEITERATORMINUS_HPP
