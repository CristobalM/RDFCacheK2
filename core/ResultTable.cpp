#include <memory>

#include "ResultTable.hpp"

ResultTable::ResultTable(unsigned long first_column_header,
                         std::vector<unsigned long> &&first_column) {
  headers.push_back(first_column_header);
  for (size_t i = 0; i < first_column.size(); i++) {
    auto v = std::vector<unsigned long>();
    v.push_back(first_column[i]);
    data.push_back(std::move(v));
  }
}

void ResultTable::join_with(unsigned long left_column_index,
                            unsigned long right_column_index,
                            ResultTable &&right) {

  auto lit = data.begin();
  auto rit = right.data.begin();

  for (size_t i = 0; i < right.headers.size(); i++) {
    if (i != right_column_index)
      headers.push_back(right.headers[i]);
  }

  while (lit != data.end() && rit != right.data.end()) {
    auto &l = *lit;
    auto &r = *rit;

    if (l[left_column_index] == r[right_column_index]) {
      for (size_t i = 0; i < right_column_index; i++) {
        l.push_back(r[i]);
      }

      for (size_t i = right_column_index + 1; i < r.size(); i++) {
        l.push_back(r[i]);
      }
      lit++;
      rit++;
    } else if (l < r) {
      auto curr = lit;
      lit++;
      data.erase(curr);
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

std::list<std::vector<unsigned long>> &ResultTable::get_data() { return data; }