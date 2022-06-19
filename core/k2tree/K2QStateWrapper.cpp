//
// Created by cristobal on 03-08-21.
//

#include "K2QStateWrapper.hpp"
K2QStateWrapper::K2QStateWrapper(uint32_t tree_depth, uint32_t cut_depth,
                                 uint32_t max_nodes_count)
    : st({}) {
  init_k2qstate(&st, tree_depth, max_nodes_count, cut_depth);
}
struct k2qstate *K2QStateWrapper::get_ptr() {
  return &st;
}
K2QStateWrapper::~K2QStateWrapper() {
  if (no_destruct)
    return;
  clean_k2qstate(&st);
}
K2QStateWrapper::K2QStateWrapper(K2QStateWrapper &&other) noexcept
    : st(other.st) {
  other.set_no_destruct();
}

K2QStateWrapper &K2QStateWrapper::operator=(K2QStateWrapper &&other) noexcept {
  st = other.st;
  other.set_no_destruct();
  return *this;
}
void K2QStateWrapper::set_no_destruct() { no_destruct = true; }
