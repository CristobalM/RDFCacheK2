//
// Created by cristobal on 25-07-21.
//

#include "RowSortConnector.hpp"
#include <query_processing/ExprProcessor.hpp>
#include <utility>

bool RowSortConnector::Comparator::operator()(const RowSortConnector &lhs,
                                              const RowSortConnector &rhs) {
  for (int i = 0; i < order_node.sort_conditions_size(); i++) {
    const auto &sort_condition = order_node.sort_conditions(i);
    auto lhs_resource = evaluators[i]->eval_resource(lhs.input_row);
    auto rhs_resource = evaluators[i]->eval_resource(rhs.input_row);
    auto cmp = lhs_resource->diff_compare(*rhs_resource);

    if (cmp != 0) {
      if (sort_condition.direction() == proto_msg::SortDirection::ASCENDING) {
        return cmp < 0;
      } else if (sort_condition.direction() ==
                 proto_msg::SortDirection::DESCENDING) {
        return cmp > 0;
      }
    }
  }
  return false;
}
RowSortConnector::Comparator::Comparator(EvalData &eval_data,
                                         const proto_msg::OrderNode &order_node)
    : eval_data(eval_data), order_node(order_node) {
  for (int i = 0; i < order_node.sort_conditions_size(); i++) {
    const auto &sort_condition = order_node.sort_conditions(i);
    auto evaluator =
        ExprProcessor(eval_data, sort_condition.expr()).create_evaluator();
    evaluator->init();
    evaluators.push_back(std::move(evaluator));
  }
}
RowSortConnector::RowSortConnector(RowSortConnector::row_t input_row)
    : input_row(std::move(input_row)) {}
std::ostream &operator<<(std::ostream &os, const RowSortConnector &data) {
  int row_size = (int)data.input_row.size();
  os.write(reinterpret_cast<const char *>(&row_size), sizeof(row_size));
  for (auto value : data.input_row) {
    os.write(reinterpret_cast<const char *>(&value), sizeof(value));
  }
  return os;
}
bool RowSortConnector::read_value(std::ifstream &ifs,
                                  RowSortConnector &next_val) {
  int row_size;
  ifs.read(reinterpret_cast<char *>(&row_size), sizeof(row_size));
  if ((ifs.rdstate() & std::ifstream::eofbit) != 0) {
    return false;
  }
  next_val.input_row = std::vector<unsigned long>(row_size, 0);
  for (int i = 0; i < row_size; i++) {
    unsigned long current;
    ifs.read(reinterpret_cast<char *>(&current), sizeof(current));
    next_val.input_row[i] = current;
  }

  return true;
}
bool RowSortConnector::operator!=(const RowSortConnector &other) const {
  return input_row != other.input_row;
}
bool RowSortConnector::operator==(const RowSortConnector &other) const {
  return input_row == other.input_row;
}
size_t RowSortConnector::size() const {
  return input_row.size() * sizeof(unsigned long);
}
std::vector<unsigned long> &RowSortConnector::get_row() { return input_row; }
