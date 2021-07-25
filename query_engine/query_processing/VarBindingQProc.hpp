//
// Created by cristobal on 21-07-21.
//

#ifndef RDFCACHEK2_VARBINDINGQPROC_HPP
#define RDFCACHEK2_VARBINDINGQPROC_HPP

#include <unordered_map>
class VarBindingQProc {
  std::unordered_map<unsigned long, unsigned long> mapping;

public:
  bool is_bound(unsigned long id);
  unsigned long get_value(unsigned long id);
  void bind(unsigned long var_id, unsigned long value_id);
};

#endif // RDFCACHEK2_VARBINDINGQPROC_HPP
