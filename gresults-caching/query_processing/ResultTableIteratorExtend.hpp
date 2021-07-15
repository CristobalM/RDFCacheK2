//
// Created by cristobal on 7/14/21.
//

#ifndef RDFCACHEK2_RESULTTABLEITERATOREXTEND_HPP
#define RDFCACHEK2_RESULTTABLEITERATOREXTEND_HPP

#include "ResultTableIterator.hpp"
#include "VarLazyBinding.hpp"
#include <memory>
#include <query_processing/expr/EvalData.hpp>
#include <vector>
class ResultTableIteratorExtend : public ResultTableIterator {

  std::shared_ptr<ResultTableIterator> input_it;
  std::unique_ptr<EvalData> eval_data;
  std::vector<std::unique_ptr<VarLazyBinding>> var_bindings;
  std::vector<unsigned long> headers;
  std::vector<unsigned long> mapping_original;
  std::vector<unsigned long> mapping_extra;

public:
  ResultTableIteratorExtend(
      std::shared_ptr<ResultTableIterator> input_it,
      std::unique_ptr<EvalData> &&eval_data,
      std::vector<std::unique_ptr<VarLazyBinding>> &&var_bindings);
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

#endif // RDFCACHEK2_RESULTTABLEITERATOREXTEND_HPP
