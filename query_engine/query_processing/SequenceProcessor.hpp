//
// Created by cristobal on 7/21/21.
//

#ifndef RDFCACHEK2_SEQUENCEPROCESSOR_HPP
#define RDFCACHEK2_SEQUENCEPROCESSOR_HPP

#include "QProc.hpp"
#include "VarBindingQProc.hpp"
#include "VarDependencyGraph.hpp"
#include "VarIndexManager.hpp"
#include "VarsCollection.hpp"
#include <sparql_tree.pb.h>

#include <query_processing/iterators/QueryIterator.hpp>
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
  std::shared_ptr<QueryIterator> execute_it();

private:
  std::vector<VarsCollection> get_vars_sequence();
  VarsCollection get_vars_from_node(const proto_msg::SparqlNode &node,
                                    int position);

  static std::vector<unsigned long>
  generate_headers(VarDependencyGraph &vd_graph);
};

#endif // RDFCACHEK2_SEQUENCEPROCESSOR_HPP
