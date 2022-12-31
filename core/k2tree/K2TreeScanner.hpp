//
// Created by cristobal on 7/22/21.
//

#ifndef RDFCACHEK2_K2TREESCANNER_HPP
#define RDFCACHEK2_K2TREESCANNER_HPP

#include <utility>

namespace k2cache {

class K2TreeMixed;

class K2TreeScanner {
public:
  enum BandType { COLUMN_BAND_TYPE = 0, ROW_BAND_TYPE = 1 };

  virtual bool has_next() = 0;
  virtual std::pair<uint64_t, uint64_t> next() = 0;
  virtual void reset_scan() = 0;

  virtual uint64_t get_band_value() = 0;

  virtual K2TreeMixed &get_tree() = 0;

  virtual ~K2TreeScanner() = default;

  virtual K2QStateWrapper &get_k2qw() = 0;
};
} // namespace k2cache

#endif // RDFCACHEK2_K2TREESCANNER_HPP
