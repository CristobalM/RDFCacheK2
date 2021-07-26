//
// Created by cristobal on 25-07-21.
//

#ifndef RDFCACHEK2_RESULTTABLEITERATORORDER_HPP
#define RDFCACHEK2_RESULTTABLEITERATORORDER_HPP

#include "ExternalSortedFileHandler.hpp"
#include "ResultTableIterator.hpp"
#include "RowSortConnector.hpp"
#include <query_processing/expr/EvalData.hpp>
#include <sparql_tree.pb.h>
class ResultTableIteratorOrder : public ResultTableIterator {
  std::shared_ptr<ResultTableIterator> input_it;
  ExternalSortedFileHandler external_sorted_file_handler;

  static constexpr size_t DEFAULT_MAX_IN_MEMORY_SIZE_SORT =
      1'000'000'000; // 1GB

public:
  ResultTableIteratorOrder(const std::shared_ptr<ResultTableIterator> &input_it,
                           const proto_msg::OrderNode &order_node,
                           EvalData &&eval_data, bool remove_duplicates,
                           TimeControl &time_control);
  bool has_next() override;
  std::vector<unsigned long> next() override;
  std::vector<unsigned long> &get_headers() override;
  void reset_iterator() override;
};

#endif // RDFCACHEK2_RESULTTABLEITERATORORDER_HPP
