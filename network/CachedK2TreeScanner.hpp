//
// Created by cristobal on 2/21/22.
//

#ifndef RDFCACHEK2_CACHEDK2TREESCANNER_HPP
#define RDFCACHEK2_CACHEDK2TREESCANNER_HPP

#include <memory>
#include <vector>

#include "K2TreeScanner.hpp"
#include "K2QStateWrapper.hpp"
#include "K2TreeMixed.hpp"
class CachedK2TreeScanner : public K2TreeScanner {
  std::vector<unsigned long> points_plain;
  unsigned long current_i;
public:
  bool has_next() override;
  std::pair<unsigned long, unsigned long> next() override;
  void reset_scan() override;
  unsigned long get_band_value() override;
  K2TreeMixed &get_tree() override;
  K2QStateWrapper &get_k2qw() override;
  unsigned long get_size() override;

  class Builder{
    std::vector<unsigned long> points_plain;
  public:
    explicit Builder(unsigned long expected_points);
    explicit Builder();
    void add_point(unsigned long col, unsigned long row);
    std::unique_ptr<K2TreeScanner> get_scanner();
  };

  CachedK2TreeScanner(std::vector<unsigned long> &&points_plain);
  bool has_point(unsigned long col, unsigned long row) override;
  bool is_cached() override;
};



#endif // RDFCACHEK2_CACHEDK2TREESCANNER_HPP
