//
// Created by cristobal on 3/2/22.
//

#include <algorithm>

#include "FullyIndexedPredicate.hpp"
FullyIndexedPredicate::FullyIndexedPredicate(const K2TreeMixed &k2tree) {
  k2tree.scan_points(
      [](unsigned long col, unsigned long row, void *report_state) {
        auto *self = reinterpret_cast<FullyIndexedPredicate *>(report_state);
        self->subject_to_object[col].push_back(row);
        self->object_to_subject[row].push_back(col);
        self->subjects.insert(col);
        self->objects.insert(row);
      },
      this);
}
bool FullyIndexedPredicate::has(unsigned long subject_id,
                                unsigned long object_id) {
  auto &so = subject_to_object[subject_id];
  auto &os = object_to_subject[object_id];
  if (so.size() <= os.size()) {
    return std::find(so.begin(), so.end(), object_id) != so.end();
  }
  return std::find(os.begin(), os.end(), subject_id) != os.end();
}
const std::vector<unsigned long> &
FullyIndexedPredicate::get_subjects(unsigned long object_value) {
  return object_to_subject[object_value];
}
const std::vector<unsigned long> &
FullyIndexedPredicate::get_objects(unsigned long subject_value) {
  return subject_to_object[subject_value];
}
const std::set<unsigned long> &FullyIndexedPredicate::get_all_subjects() {
  return subjects;
}
const std::set<unsigned long> &FullyIndexedPredicate::get_all_objects() {
  return objects;
}
