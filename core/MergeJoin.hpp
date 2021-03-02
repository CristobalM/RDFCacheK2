#ifndef MERGE_JOIN_HPP
#define MERGE_JOIN_HPP

#include "K2Tree.hpp"
#include <vector>

namespace MergeJoin {
using vul_t = std::vector<unsigned long>;

vul_t merge_vectors(vul_t &left, vul_t &right) {
  vul_t result;
  size_t li = 0;
  size_t ri = 0;
  while (li < left.size() && ri < right.size()) {
    auto l = left[li];
    auto r = right[ri];

    if (l == r) {
      result.push_back(l);
      li++;
      ri++;
    } else if (l < r) {
      li++;
    } else {
      ri++;
    }
  }

  return result;
}
/*
vul_t ss_join(K2Tree &left, unsigned long left_object, K2Tree &right,
              unsigned long right_object) {
  auto left_row = left.get_row(left_object);
  auto right_row = right.get_row(right_object);
  return merge_vectors(left_row, right_row);
}

vul_t oo_join(K2Tree &left, unsigned long left_subject, K2Tree &right,
              unsigned long right_subject) {
  auto left_column = left.get_column(left_subject);
  auto right_column = right.get_column(right_subject);
  return merge_vectors(left_column, right_column);
}

vul_t so_join(K2Tree &left, unsigned long left_object, K2Tree &right,
              unsigned long right_subject) {
  auto left_row = left.get_row(left_object);
  auto right_column = right.get_column(right_subject);
  return merge_vectors(left_row, right_column);
}
*/

} // namespace MergeJoin

#endif