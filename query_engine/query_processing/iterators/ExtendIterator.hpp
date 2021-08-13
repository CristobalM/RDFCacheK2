//
// Created by cristobal on 7/14/21.
//

#ifndef RDFCACHEK2_EXTENDITERATOR_HPP
#define RDFCACHEK2_EXTENDITERATOR_HPP

#include "QueryIterator.hpp"
#include <TimeControl.hpp>
#include <memory>
#include <query_processing/VarLazyBinding.hpp>
#include <query_processing/expr/EvalData.hpp>
#include <vector>
class ExtendIterator : public QueryIterator {

  std::shared_ptr<QueryIterator> input_it;
  std::unique_ptr<EvalData> eval_data;
  std::vector<std::unique_ptr<VarLazyBinding>> var_bindings;
  std::vector<unsigned long> headers;
  std::vector<unsigned long> mapping_original;
  std::vector<unsigned long> mapping_extra;

public:
  ExtendIterator(std::shared_ptr<QueryIterator> input_it,
                 std::unique_ptr<EvalData> &&eval_data,
                 std::vector<std::unique_ptr<VarLazyBinding>> &&var_bindings,
                 TimeControl &time_control);
  bool has_next() override;
  std::vector<unsigned long> next() override;
  std::vector<unsigned long> &get_headers() override;
  void reset_iterator() override;
  std::vector<unsigned long> build_headers();
  std::vector<unsigned long> next_concrete();
  std::vector<unsigned long> build_mapping_original();
  std::vector<unsigned long> build_mapping_extra();
  std::vector<unsigned long>
  get_transformed_row(std::vector<unsigned long> &input_row);
  unsigned long get_rdf_resource_id_value(RDFResource &resource);
};

#endif // RDFCACHEK2_EXTENDITERATOR_HPP
