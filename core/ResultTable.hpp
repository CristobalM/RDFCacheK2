#ifndef RESULT_TABLE_HPP
#define RESULT_TABLE_HPP

#include <list>
#include <unordered_map>
#include <vector>

class ResultTable {
  using vul_t = std::vector<unsigned long>;
  using lvul_t = std::list<vul_t>;

  vul_t headers;
  lvul_t data;

public:
  ResultTable(unsigned long first_column_header,
              std::vector<unsigned long> &&first_column);

  void join_with(unsigned long left_column_index, unsigned long right_index,
                 ResultTable &&right);
  lvul_t &get_data();
};

#endif