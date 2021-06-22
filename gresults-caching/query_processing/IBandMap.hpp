//
// Created by cristobal on 6/20/21.
//

#ifndef RDFCACHEK2_IBANDMAP_HPP
#define RDFCACHEK2_IBANDMAP_HPP

#include <vector>
class IBandMap {
public:
  enum BType { BY_COL = 0, BY_ROW = 1 };

  virtual const std::vector<unsigned long> &get_band(unsigned long key) = 0;
  virtual ~IBandMap() = default;
};

#endif // RDFCACHEK2_IBANDMAP_HPP
