//
// Created by cristobal on 3/2/22.
//

#include <algorithm>

#include "FullyIndexedPredicate.hpp"
namespace k2cache {
FullyIndexedPredicate::FullyIndexedPredicate(const K2TreeMixed &k2tree) {
  k2tree.scan_points(
      [](uint64_t col, uint64_t row, void *report_state) {
        auto *self = reinterpret_cast<FullyIndexedPredicate *>(report_state);
        self->subject_to_object[col].push_back(row);
        self->object_to_subject[row].push_back(col);
        self->subjects.insert(col);
        self->objects.insert(row);
      },
      this);
}
bool FullyIndexedPredicate::has(uint64_t subject_id,
                                uint64_t object_id) {
  auto &so = subject_to_object[subject_id];
  auto &os = object_to_subject[object_id];
  if (so.size() <= os.size()) {
    return std::find(so.begin(), so.end(), object_id) != so.end();
  }
  return std::find(os.begin(), os.end(), subject_id) != os.end();
}
const std::vector<uint64_t> &
FullyIndexedPredicate::get_subjects(uint64_t object_value) {
  return object_to_subject[object_value];
}
const std::vector<uint64_t> &
FullyIndexedPredicate::get_objects(uint64_t subject_value) {
  return subject_to_object[subject_value];
}
const std::set<uint64_t> &FullyIndexedPredicate::get_all_subjects() {
  return subjects;
}
const std::set<uint64_t> &FullyIndexedPredicate::get_all_objects() {
  return objects;
}
} // namespace k2cache