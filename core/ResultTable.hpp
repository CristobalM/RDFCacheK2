#ifndef RESULT_TABLE_HPP
#define RESULT_TABLE_HPP

#include <list>
#include <unordered_map>
#include <vector>

class ResultTable {
  using vul_t = std::vector<unsigned long>;
  using lvul_t = std::list<vul_t>;
  using map_t = std::unordered_map<unsigned long, unsigned long>;

  vul_t headers;
  map_t headers_map;
  lvul_t data;

public:
  ResultTable();

  ResultTable(unsigned long first_column_header,
              std::vector<unsigned long> &&first_column);

  ResultTable(unsigned long first_column_header,
              std::list<vul_t> &&initial_list);

  void left_inner_join_with(unsigned long left_column_index,
                            unsigned long right_index, ResultTable &right);

  void left_outer_join_with(unsigned long left_column_index,
                            unsigned long right_index, ResultTable &right);

  lvul_t &get_data();

private:
  void join_with(unsigned long left_column_index, unsigned long right_index,
                 ResultTable &right, bool outer_join);
};

#endif