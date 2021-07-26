//
// Created by cristobal on 25-07-21.
//

#ifndef RDFCACHEK2_ROWSORTCONNECTOR_HPP
#define RDFCACHEK2_ROWSORTCONNECTOR_HPP

#include <query_processing/expr/EvalData.hpp>
#include <query_processing/expr/ExprEval.hpp>
#include <sparql_tree.pb.h>
#include <vector>
class RowSortConnector {

public:
  using row_t = std::vector<unsigned long>;

  explicit RowSortConnector(std::vector<unsigned long> input_row);
  RowSortConnector() = default;
  RowSortConnector(const RowSortConnector &other) = default;
  RowSortConnector(RowSortConnector &&other) noexcept = default;
  RowSortConnector &operator=(const RowSortConnector &other) = default;
  RowSortConnector &operator=(RowSortConnector &&other) noexcept = default;

  struct Comparator {
    EvalData &eval_data;
    const proto_msg::OrderNode &order_node;
    std::vector<std::unique_ptr<ExprEval>> evaluators;

    Comparator(EvalData &eval_data, const proto_msg::OrderNode &order_node);
    bool operator()(const RowSortConnector &lhs, const RowSortConnector &rhs);
  };

  friend std::ostream &operator<<(std::ostream &os,
                                  const RowSortConnector &data);
  bool operator==(const RowSortConnector &other) const;
  bool operator!=(const RowSortConnector &other) const;
  static bool read_value(std::ifstream &ifs, RowSortConnector &next_val);

  size_t size() const;
  std::vector<unsigned long> &get_row();

private:
  row_t input_row;
};

#endif // RDFCACHEK2_ROWSORTCONNECTOR_HPP
