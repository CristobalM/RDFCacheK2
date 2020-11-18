#include <memory>

#include "ResultTable.hpp"

ResultTable::ResultTable(){}

ResultTable::ResultTable(unsigned long first_column_header,
                         std::vector<unsigned long> &&first_column) {
  headers.push_back(first_column_header);
  for (size_t i = 0; i < first_column.size(); i++) {
    auto v = std::vector<unsigned long>();
    v.push_back(first_column[i]);
    data.push_back(std::move(v));
  }
}

ResultTable::ResultTable(unsigned long first_column_header,
                         std::list<vul_t> &&initial_list)
    : data(std::move(initial_list)) {
  headers.push_back(first_column_header);
}

void ResultTable::join_with(unsigned long left_column_index,
                            unsigned long right_column_index,
                            ResultTable &right, bool outer) {

  auto lit = data.begin();
  auto rit = right.data.begin();

  for (size_t i = 0; i < right.headers.size(); i++) {
    if (i != right_column_index)
      headers.push_back(right.headers[i]);
  }

  while (lit != data.end() && rit != right.data.end()) {
    auto &l = *lit;
    auto &r = *rit;

    auto lval = l[left_column_index];
    auto rval = r[right_column_index];

    if (lval == rval) {
      for (size_t i = 0; i < right_column_index; i++) {
        l.push_back(r[i]);
      }

      for (size_t i = right_column_index + 1; i < r.size(); i++) {
        l.push_back(r[i]);
      }
      lit++;
      rit++;
    } else if (lval < rval) {
      auto curr = lit;
      lit++;
      if (!outer)
        data.erase(curr);
      else
        l.push_back(0);
    } else {
      rit++;
    }
  }

  while (lit != data.end()) {
    auto curr = lit;
    lit++;
    data.erase(curr);
  }
}

void ResultTable::left_outer_join_with(unsigned long left_column_index,
                                       unsigned long right_index,
                                       ResultTable &right) {
  join_with(left_column_index, right_index, right, true);
}

void ResultTable::left_inner_join_with(unsigned long left_column_index,
                                       unsigned long right_index,
                                       ResultTable &right) {
  join_with(left_column_index, right_index, right, false);
}

std::list<std::vector<unsigned long>> &ResultTable::get_data() { return data; }