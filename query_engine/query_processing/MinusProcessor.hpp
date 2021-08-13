//
// Created by cristobal on 25-07-21.
//

#ifndef RDFCACHEK2_MINUSPROCESSOR_HPP
#define RDFCACHEK2_MINUSPROCESSOR_HPP

#include "QProc.hpp"
#include "iterators/QueryIterator.hpp"
#include <sparql_tree.pb.h>
class MinusProcessor {

  const proto_msg::MinusNode &minus_node;
  QProc *query_processor;
  TimeControl &time_control;
  std::shared_ptr<VarBindingQProc> var_binding_qproc;

public:
  MinusProcessor(const proto_msg::MinusNode &minus_node, QProc *query_processor,
                 TimeControl &time_control,
                 std::shared_ptr<VarBindingQProc> var_binding_qproc);
  std::shared_ptr<QueryIterator> execute_it();
  static bool
  with_shared_headers(const std::vector<unsigned long> &left_headers,
                      std::set<unsigned long> &right_vars_set);
  std::set<unsigned long> get_right_vars_set();
};

#endif // RDFCACHEK2_MINUSPROCESSOR_HPP
