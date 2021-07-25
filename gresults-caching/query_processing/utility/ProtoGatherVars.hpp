//
// Created by cristobal on 25-07-21.
//

#ifndef RDFCACHEK2_PROTOGATHERVARS_HPP
#define RDFCACHEK2_PROTOGATHERVARS_HPP

#include "GatheredVars.hpp"
#include <query_processing/VarIndexManager.hpp>
#include <sparql_tree.pb.h>
class ProtoGatherVars {
public:
  static GatheredVars get_vars_from_node(VarIndexManager &vim,
                                         const proto_msg::SparqlNode &node);

private:
  static void get_vars_from_node_rec(const proto_msg::SparqlNode &node,
                                     std::set<unsigned long> &table_vars,
                                     std::set<unsigned long> &ref_vars,
                                     bool mark_table_vars_as_ref,
                                     VarIndexManager &vim);
  static void add_vars_from_triple(const proto_msg::TripleNode &triple_node,
                                   std::set<unsigned long> &table_vars,
                                   std::set<unsigned long> &ref_vars,
                                   bool mark_table_vars_as_ref,
                                   VarIndexManager &vim);

  static void get_vars_from_expr_rec(const proto_msg::ExprNode &expr_node,
                                     std::set<unsigned long> &table_vars,
                                     std::set<unsigned long> &ref_vars,
                                     VarIndexManager &vim);

  static void
  add_vars_from_assignment(const proto_msg::AssignmentNode &assignment_node,
                           std::set<unsigned long> &table_vars,
                           std::set<unsigned long> &ref_vars,
                           bool mark_table_vars_as_ref, VarIndexManager &vim);
  static void cond_add_var(const std::string &var_str,
                           std::set<unsigned long> &table_vars,
                           std::set<unsigned long> &ref_vars,
                           bool mark_table_vars_as_ref, VarIndexManager &vim);
};

#endif // RDFCACHEK2_PROTOGATHERVARS_HPP
