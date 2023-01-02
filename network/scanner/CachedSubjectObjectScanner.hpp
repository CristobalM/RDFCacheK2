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
  //  static constexpr uint64_t buf_sz = 4096;
  //  std::array<uint64_t, buf_sz> buf{};

  I_CachedPredicateSource *cached_source;

  bool left_right_dir;

  const std::set<uint64_t> &base_set;

  //  size_t current_buf_element;

  std::set<uint64_t>::iterator base_set_it;
  std::vector<uint64_t> const *active_right_elements;
  uint64_t current_base_element;
  uint64_t current_active_element_position;

  bool finished;

public:
  explicit CachedSubjectObjectScanner(I_CachedPredicateSource *cached_source);
  bool has_next() override;
  std::pair<uint64_t, uint64_t> next() override;

private:
  void check_active_elements();
  const std::vector<uint64_t> *get_active_elements();
};
} // namespace k2cache

#endif // RDFCACHEK2_CACHEDSUBJECTOBJECTSCANNER_HPP
