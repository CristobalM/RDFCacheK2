//
// Created by cristobal on 20-07-21.
//

#ifndef RDFCACHEK2_RESULTTABLEVECTOR_HPP
#define RDFCACHEK2_RESULTTABLEVECTOR_HPP

#include <vector>
struct ResultTableVector {
  using vul_t = std::vector<unsigned long>;
  using vvul_t = std::vector<vul_t>;

  vul_t headers;
  vvul_t data;
};

#endif // RDFCACHEK2_RESULTTABLEVECTOR_HPP
