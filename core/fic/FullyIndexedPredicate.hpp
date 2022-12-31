//
// Created by cristobal on 3/2/22.
//

#ifndef RDFCACHEK2_FULLYINDEXEDPREDICATE_HPP
#define RDFCACHEK2_FULLYINDEXEDPREDICATE_HPP

#include <set>
#include <unordered_map>
#include <vector>

#include "I_CachedPredicateSource.hpp"
#include "k2tree/K2TreeMixed.hpp"

namespace k2cache {
// This is a very space-expensive structure, optimized only for speed
class FullyIndexedPredicate : public I_CachedPredicateSource {
  std::unordered_map<uint64_t, std::vector<uint64_t>>
      subject_to_object;
  std::unordered_map<uint64_t, std::vector<uint64_t>>
      object_to_subject;
  std::set<uint64_t> subjects;
  std::set<uint64_t> objects;

public:
  explicit FullyIndexedPredicate(const K2TreeMixed &k2tree);
  bool has(uint64_t subject_id, uint64_t object_id) override;
  const std::vector<uint64_t> &
  get_subjects(uint64_t object_value) override;
  const std::vector<uint64_t> &
  get_objects(uint64_t subject_value) override;
  const std::set<uint64_t> &get_all_subjects() override;
  const std::set<uint64_t> &get_all_objects() override;
};
} // namespace k2cache

#endif // RDFCACHEK2_FULLYINDEXEDPREDICATE_HPP
