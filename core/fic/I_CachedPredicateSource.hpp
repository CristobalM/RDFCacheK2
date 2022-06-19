//
// Created by cristobal on 3/2/22.
//

#ifndef RDFCACHEK2_I_CACHEDPREDICATESOURCE_HPP
#define RDFCACHEK2_I_CACHEDPREDICATESOURCE_HPP

#include <set>
#include <vector>
struct I_CachedPredicateSource {
  virtual ~I_CachedPredicateSource() = default;

  virtual bool has(unsigned long subject_id, unsigned long object_id) = 0;
  virtual const std::vector<unsigned long> &
  get_subjects(unsigned long object_value) = 0;
  virtual const std::vector<unsigned long> &
  get_objects(unsigned long subject_value) = 0;
  virtual const std::set<unsigned long> &get_all_subjects() = 0;
  virtual const std::set<unsigned long> &get_all_objects() = 0;
};

#endif // RDFCACHEK2_I_CACHEDPREDICATESOURCE_HPP
