//
// Created by cristobal on 8/14/21.
//

#ifndef RDFCACHEK2_I_PQTRAVERSE_HPP
#define RDFCACHEK2_I_PQTRAVERSE_HPP

#include <cstdint>

namespace k2cache {
struct I_PQTraverse {
  virtual ~I_PQTraverse() = default;
  virtual bool has_next() = 0;
  virtual uint64_t next_key() = 0;
};
} // namespace k2cache

#endif // RDFCACHEK2_I_PQTRAVERSE_HPP
