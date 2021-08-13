//
// Created by cristobal on 25-07-21.
//

#ifndef RDFCACHEK2_OPTIONALPROCESSOR_HPP
#define RDFCACHEK2_OPTIONALPROCESSOR_HPP

#include "QProc.hpp"
#include "VarBindingQProc.hpp"
#include <sparql_tree.pb.h>
class OptionalProcessor {
  const proto_msg::SparqlNode &left_node;
  const proto_msg::SparqlNode &right_node;
  QProc *query_processor;
  std::shared_ptr<VarBindingQProc> var_binding_qproc;
  TimeControl &time_control;

public:
  OptionalProcessor(const proto_msg::SparqlNode &left_node,
                    const proto_msg::SparqlNode &right_node,
                    QProc *query_processor,
                    std::shared_ptr<VarBindingQProc> var_binding_qproc,
                    TimeControl &time_control);
  std::shared_ptr<QueryIterator> execute_it();
  std::set<unsigned long> get_right_table_vars_set();
};

#endif // RDFCACHEK2_OPTIONALPROCESSOR_HPP
