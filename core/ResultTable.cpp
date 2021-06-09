#include <algorithm>
#include <cassert>
#include <memory>
#include <stdexcept>

#include "ResultTable.hpp"

ResultTable::ResultTable() {}

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

void ResultTable::left_join_with(unsigned long left_column_index,
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
        for (size_t i = 0; i < right.headers.size() - 1; i++)
          l.push_back(0);
    } else {
      rit++;
    }
  }

  while (lit != data.end()) {
    auto curr = lit;
    auto &l = *lit;
    lit++;
    if (!outer)
      data.erase(curr);
    else
      for (size_t i = 0; i < right.headers.size() - 1; i++)
        ;
    l.push_back(0);
  }
}

void ResultTable::left_join_with_vector(unsigned long left_column_index,
                                        std::vector<unsigned long> &vec,
                                        bool outer) {
  auto lit = data.begin();
  auto rit = vec.begin();

  while (lit != data.end() && rit != vec.end()) {
    auto &l = *lit;

    auto lval = l[left_column_index];
    auto rval = *rit;

    if (lval == rval) {
      lit++;
      rit++;
    } else if (lval < rval) {
      auto curr = lit;
      lit++;
      if (!outer)
        data.erase(curr);
    } else {
      rit++;
    }
  }

  while (lit != data.end()) {
    auto curr = lit;
    lit++;
    if (!outer)
      data.erase(curr);
  }
}

void ResultTable::left_outer_join_with(unsigned long left_column_index,
                                       unsigned long right_index,
                                       ResultTable &right) {
  left_join_with(left_column_index, right_index, right, true);
}

void ResultTable::left_inner_join_with(unsigned long left_column_index,
                                       unsigned long right_index,
                                       ResultTable &right) {
  left_join_with(left_column_index, right_index, right, false);
}

std::list<std::vector<unsigned long>> &ResultTable::get_data() { return data; }

std::size_t ResultTable::rows_size() const { return data.size(); }

unsigned long ResultTable::get_actual_index(unsigned long virtual_index) {
  for (size_t i = 0; i < headers.size(); i++)
    if (headers[i] == virtual_index)
      return i;
  throw std::runtime_error("Virtual index " + std::to_string(virtual_index) +
                           " not found on headers list");
}

void ResultTable::add_row(std::vector<unsigned long> &&row) {
  data.push_back(std::move(row));
}

ResultTable::ResultTable(std::vector<unsigned long> &&headers)
    : headers(std::move(headers)) {}

ResultTable::ResultTable(std::vector<unsigned long> &headers)
    : headers(headers) {}

void ResultTable::sort_rows() {
  data.sort([](const vul_t &lhs, const vul_t &rhs) {
    assert(lhs.size() == rhs.size());

    for (size_t i = 0; i < lhs.size(); i++) {
      unsigned long lhs_i = lhs.at(i);
      unsigned long rhs_i = rhs.at(i);
      if (lhs_i == rhs_i)
        continue;
      return lhs_i < rhs_i;
    }
    return false;
  });
}
