//
// Created by cristobal on 25-07-21.
//

#ifndef RDFCACHEK2_LEFTOUTERJOINITERATOR_HPP
#define RDFCACHEK2_LEFTOUTERJOINITERATOR_HPP

#include "QueryIterator.hpp"
#include <NaiveDynamicStringDictionary.hpp>
#include <PredicatesCacheManager.hpp>
#include <query_processing/VarBindingQProc.hpp>
#include <query_processing/VarIndexManager.hpp>
#include <sparql_tree.pb.h>
class LeftOuterJoinIterator : public QueryIterator {

  std::shared_ptr<QueryIterator> left_it;
  const proto_msg::SparqlNode right_node;
  std::shared_ptr<VarBindingQProc> var_binding_qproc;

  std::vector<unsigned long> headers;
  std::shared_ptr<VarBindingQProc> current_var_binding_qproc;
  std::shared_ptr<QueryIterator> current_right_it;
  std::vector<unsigned long> left_header_pos_map;
  std::vector<unsigned long> right_header_pos_map;

  std::vector<unsigned long> tmp_holder;
  std::vector<unsigned long> next_value;

  std::vector<unsigned long> current_left_row;

  bool next_available;
  bool left_row_active;

  std::shared_ptr<PredicatesCacheManager> cache_manager;
  std::shared_ptr<VarIndexManager> vim;
  std::shared_ptr<NaiveDynamicStringDictionary> extra_str_dict;

  std::vector<unsigned long> right_positions;
  const std::string &temp_files_dir;

public:
  LeftOuterJoinIterator(
      std::shared_ptr<QueryIterator> left_it, proto_msg::SparqlNode right_node,
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
  void fill_right_rows_with_zero();
  std::vector<unsigned long>
  build_right_positions(std::vector<unsigned long> &left_headers);
};

#endif // RDFCACHEK2_LEFTOUTERJOINITERATOR_HPP
