//
// Created by cristobal on 03-08-21.
//

#ifndef RDFCACHEK2_K2QSTATEWRAPPER_HPP
#define RDFCACHEK2_K2QSTATEWRAPPER_HPP

extern "C" {
#include <k2node.h>
}

#include <cstdint>

class K2QStateWrapper {
  struct k2qstate st;

public:
  K2QStateWrapper(uint32_t tree_depth, uint32_t cut_depth,
                  uint32_t max_nodes_count);
  struct k2qstate *get_ptr();
  ~K2QStateWrapper();
};

#endif // RDFCACHEK2_K2QSTATEWRAPPER_HPP
