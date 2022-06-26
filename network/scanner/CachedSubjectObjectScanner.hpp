//
// Created by cristobal on 3/2/22.
//

#ifndef RDFCACHEK2_CACHEDSUBJECTOBJECTSCANNER_HPP
#define RDFCACHEK2_CACHEDSUBJECTOBJECTSCANNER_HPP

#include "CachedSourceScanner.hpp"
#include "fic/I_CachedPredicateSource.hpp"
#include <array>
namespace k2cache {
class CachedSubjectObjectScanner : public CachedSourceScanner {
  //  static constexpr unsigned long buf_sz = 4096;
  //  std::array<unsigned long, buf_sz> buf{};

  I_CachedPredicateSource *cached_source;

  bool left_right_dir;

  const std::set<unsigned long> &base_set;

  //  size_t current_buf_element;

  std::set<unsigned long>::iterator base_set_it;
  std::vector<unsigned long> const *active_right_elements;
  unsigned long current_base_element;
  unsigned long current_active_element_position;

  bool finished;

public:
  explicit CachedSubjectObjectScanner(I_CachedPredicateSource *cached_source);
  bool has_next() override;
  std::pair<unsigned long, unsigned long> next() override;

private:
  void check_active_elements();
  const std::vector<unsigned long> *get_active_elements();
};
} // namespace k2cache

#endif // RDFCACHEK2_CACHEDSUBJECTOBJECTSCANNER_HPP
