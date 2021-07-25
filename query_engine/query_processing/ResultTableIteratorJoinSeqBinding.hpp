//
// Created by cristobal on 21-07-21.
//

#ifndef RDFCACHEK2_RESULTTABLEITERATORJOINSEQBINDING_HPP
#define RDFCACHEK2_RESULTTABLEITERATORJOINSEQBINDING_HPP

#include "QProc.hpp"
#include "ResultTableIterator.hpp"
#include <PredicatesCacheManager.hpp>
#include <sparql_tree.pb.h>
class ResultTableIteratorJoinSeqBinding : public ResultTableIterator {

  std::shared_ptr<ResultTableIterator> input_it;
  const proto_msg::SparqlNode proto_node;
  std::shared_ptr<VarBindingQProc> var_binding_qproc;

  std::vector<unsigned long> headers;
  std::shared_ptr<VarBindingQProc> current_var_binding_qproc;
  std::shared_ptr<ResultTableIterator> current_right_it;
  std::vector<unsigned long> left_header_pos_map;
  std::vector<unsigned long> right_header_pos_map;

  std::vector<unsigned long> tmp_holder;
  bool next_available;
  std::vector<unsigned long> next_value;

  std::vector<unsigned long> current_left_row;
  bool left_row_active;
  std::shared_ptr<PredicatesCacheManager> cache_manager;
  std::shared_ptr<VarIndexManager> vim;

  std::shared_ptr<NaiveDynamicStringDictionary> extra_str_dict;

public:
  ResultTableIteratorJoinSeqBinding(
      std::shared_ptr<ResultTableIterator> input_it,
      proto_msg::SparqlNode proto_node,
      std::shared_ptr<VarBindingQProc> var_binding_qproc,
      std::set<unsigned long> &right_table_vars, TimeControl &time_control,
      std::shared_ptr<PredicatesCacheManager> cache_manager,
      std::shared_ptr<VarIndexManager> vim,
      std::shared_ptr<NaiveDynamicStringDictionary> extra_str_dict);
  bool has_next() override;
  std::vector<unsigned long> next() override;
  std::vector<unsigned long> &get_headers() override;
  void reset_iterator() override;
  void build_headers(std::set<unsigned long> &right_table_vars);
  bool create_var_binding_qproc_if_needed(std::vector<unsigned long> &left_row);
  std::vector<unsigned long> next_concrete();
  static std::vector<unsigned long>
  get_pos_map(std::vector<unsigned long> &from, std::vector<unsigned long> &to);
  void map_rows_to_holder(std::vector<unsigned long> &source_data,
                          std::vector<unsigned long> &headers_mapping);
};

#endif // RDFCACHEK2_RESULTTABLEITERATORJOINSEQBINDING_HPP
