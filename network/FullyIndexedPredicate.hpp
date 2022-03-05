//
// Created by cristobal on 3/2/22.
//

#ifndef RDFCACHEK2_FULLYINDEXEDPREDICATE_HPP
#define RDFCACHEK2_FULLYINDEXEDPREDICATE_HPP

#include <set>
#include <unordered_map>

#include "I_CachedPredicateSource.hpp"
#include "K2TreeMixed.hpp"

// This is a very space-expensive structure, optimized only for speed
class FullyIndexedPredicate : public I_CachedPredicateSource {
  std::unordered_map<unsigned long, std::vector<unsigned long>>
      subject_to_object;
  std::unordered_map<unsigned long, std::vector<unsigned long>>
      object_to_subject;
  std::set<unsigned long> subjects;
  std::set<unsigned long> objects;

public:
  explicit FullyIndexedPredicate(const K2TreeMixed &k2tree);
  bool has(unsigned long subject_id, unsigned long object_id) override;
  const std::vector<unsigned long> &
  get_subjects(unsigned long object_value) override;
  const std::vector<unsigned long> &
  get_objects(unsigned long subject_value) override;
  const std::set<unsigned long> &get_all_subjects() override;
  const std::set<unsigned long> &get_all_objects() override;
};

#endif // RDFCACHEK2_FULLYINDEXEDPREDICATE_HPP
