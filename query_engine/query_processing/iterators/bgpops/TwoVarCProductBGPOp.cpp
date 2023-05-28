//
// Created by cristobal on 7/12/21.
//

#include "TwoVarCProductBGPOp.hpp"
namespace k2cache {

TwoVarCProductBGPOp::TwoVarCProductBGPOp(
    std::unique_ptr<K2TreeScanner> &&scanner, unsigned long subject_pos,
    unsigned long object_pos, TimeControl &time_control)
    : scanner(std::move(scanner)), subject_pos(subject_pos),
      object_pos(object_pos), time_control(time_control) {}
BGPOp::RunResult
TwoVarCProductBGPOp::run(std::vector<unsigned long> &row_to_fill) {
  BGPOp::RunResult result;
  if (!time_control.tick())
    return result;

  if (!scanner->has_next()) {
    result.scan_done = true;
    result.valid_value = false;
    return result;
  };

  auto next_pair = scanner->next();
  if (!time_control.tick())
    return result;
  row_to_fill[subject_pos] = next_pair.first;
  row_to_fill[object_pos] = next_pair.second;

  result.scan_done = !scanner->has_next();
  result.valid_value = true;

  return result;
}
void TwoVarCProductBGPOp::reset_op() { scanner->reset_scan(); }
K2TreeScanner &TwoVarCProductBGPOp::get_scanner() { return *scanner; }
}
