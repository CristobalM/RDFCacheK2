//
// Created by cristobal on 8/14/21.
//

#ifndef RDFCACHEK2_I_PQTRAVERSE_HPP
#define RDFCACHEK2_I_PQTRAVERSE_HPP

struct I_PQTraverse {
  virtual ~I_PQTraverse() = default;
  virtual bool has_next() = 0;
  virtual unsigned long next_key() = 0;
};

#endif // RDFCACHEK2_I_PQTRAVERSE_HPP
