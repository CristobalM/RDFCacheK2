//
// Created by cristobal on 21-07-21.
//

#ifndef RDFCACHEK2_INNERJOINITERATOR_HPP
#define RDFCACHEK2_INNERJOINITERATOR_HPP

#include "QueryIterator.hpp"
#include <PredicatesCacheManager.hpp>
#include <query_processing/VarBindingQProc.hpp>
#include <query_processing/VarIndexManager.hpp>
#include <sparql_tree.pb.h>
class InnerJoinIterator : public QueryIterator {

  std::shared_ptr<QueryIterator> input_it;
  const proto_msg::SparqlNode proto_node;
  std::shared_ptr<VarBindingQProc> var_binding_qproc;

  std::vector<unsigned long> headers;
  std::shared_ptr<VarBindingQProc> current_var_binding_qproc;
  std::shared_ptr<QueryIterator> current_right_it;
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
  const std::string &temp_files_dir;

public:
  InnerJoinIterator(
      std::shared_ptr<QueryIterator> input_it, proto_msg::SparqlNode proto_node,
      std::shared_ptr<VarBindingQProc> var_binding_qproc,
      std::set<unsigned long> &right_table_vars, TimeControl &time_control,
      std::shared_ptr<PredicatesCacheManager> cache_manager,
      std::shared_ptr<VarIndexManager> vim,
      std::shared_ptr<NaiveDynamicStringDictionary> extra_str_dict,
      const std::string &temp_files_dir);
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

#endif // RDFCACHEK2_INNERJOINITERATOR_HPP
