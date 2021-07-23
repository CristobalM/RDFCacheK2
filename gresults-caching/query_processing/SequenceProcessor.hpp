//
// Created by cristobal on 7/21/21.
//

#ifndef RDFCACHEK2_SEQUENCEPROCESSOR_HPP
#define RDFCACHEK2_SEQUENCEPROCESSOR_HPP

#include "QProc.hpp"
#include "ResultTableIterator.hpp"
#include "VarBindingQProc.hpp"
#include "VarDependencyGraph.hpp"
#include "VarIndexManager.hpp"
#include "VarsCollection.hpp"
#include <sparql_tree.pb.h>

#include <set>
#include <vector>

class SequenceProcessor {
  const proto_msg::SequenceNode &sequence_node;
  QProc *query_processor;

  std::vector<std::vector<int>> cc_positions;
  std::vector<std::vector<std::set<unsigned long>>> cc_sets;

  std::vector<unsigned long> resulting_headers;
  TimeControl &time_control;

  std::shared_ptr<VarBindingQProc> var_binding_qproc;

public:
  SequenceProcessor(const proto_msg::SequenceNode &sequence_node,
                    QProc *query_processor, TimeControl &time_control,
                    std::shared_ptr<VarBindingQProc> var_binding_qproc);
  std::shared_ptr<ResultTableIterator> execute_it();

private:
  std::vector<VarsCollection> get_vars_sequence();
  VarsCollection get_vars_from_node(const proto_msg::SparqlNode &node,
                                    int position);
  void get_vars_from_node_rec(const proto_msg::SparqlNode &node,
                              VarsCollection &vars_collection,
                              bool mark_table_vars_as_ref);
  static void add_vars_from_triple(const proto_msg::TripleNode &triple_node,
                                   VarsCollection &vars_collection,
                                   bool mark_table_vars_as_ref);
  void get_vars_from_expr_rec(const proto_msg::ExprNode &expr_node,
                              VarsCollection &vars_collection);
  void
  add_vars_from_assignment(const proto_msg::AssignmentNode &assignment_node,
                           VarsCollection &vars_collection,
                           bool mark_table_vars_as_ref);
  static void cond_add_var(const std::string &var_str,
                           VarsCollection &vars_collection,
                           bool mark_table_vars_as_ref);
  static std::vector<unsigned long>
  generate_headers(VarDependencyGraph &vd_graph);
};

#endif // RDFCACHEK2_SEQUENCEPROCESSOR_HPP
