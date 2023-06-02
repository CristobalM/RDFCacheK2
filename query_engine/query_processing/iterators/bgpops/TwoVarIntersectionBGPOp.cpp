//
// Created by cristobal on 7/12/21.
//

#include "TwoVarIntersectionBGPOp.hpp"
#include "k2tree/K2TreeMixed.hpp"
namespace k2cache {

TwoVarIntersectionBGPOp::TwoVarIntersectionBGPOp(
    std::unique_ptr<K2TreeScanner> &&scanner, unsigned long subject_pos,
    unsigned long object_pos, TimeControl &time_control)
    : scanner(std::move(scanner)), subject_pos(subject_pos),
      object_pos(object_pos), time_control(time_control) {}
BGPOp::RunResult
TwoVarIntersectionBGPOp::run(std::vector<unsigned long> &row_to_fill) {
  BGPOp::RunResult result{};
  if (!time_control.tick())
    return result;
  auto intersect_value_subject = row_to_fill[subject_pos];
  auto intersect_value_object = row_to_fill[object_pos];
  auto &k2tree = scanner->get_tree();
  if (k2tree.has(intersect_value_subject, intersect_value_object,
                 scanner->get_k2qw())) {
    result.valid_value = true;
    result.scan_done = true;
    return result;
  }

  result.scan_done = true;
  result.valid_value = false;
  return result;
}
void TwoVarIntersectionBGPOp::reset_op() {}
K2TreeScanner &TwoVarIntersectionBGPOp::get_scanner() { return *scanner; }
}
