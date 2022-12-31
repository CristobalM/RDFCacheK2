//
// Created by cristobal on 3/2/22.
//

#ifndef RDFCACHEK2_I_CACHEDPREDICATESOURCE_HPP
#define RDFCACHEK2_I_CACHEDPREDICATESOURCE_HPP

#include <set>
#include <vector>
namespace k2cache {
struct I_CachedPredicateSource {
  virtual ~I_CachedPredicateSource() = default;

  virtual bool has(uint64_t subject_id, uint64_t object_id) = 0;
  virtual const std::vector<uint64_t> &
  get_subjects(uint64_t object_value) = 0;
  virtual const std::vector<uint64_t> &
  get_objects(uint64_t subject_value) = 0;
  virtual const std::set<uint64_t> &get_all_subjects() = 0;
  virtual const std::set<uint64_t> &get_all_objects() = 0;
};
} // namespace k2cache
#endif // RDFCACHEK2_I_CACHEDPREDICATESOURCE_HPP
