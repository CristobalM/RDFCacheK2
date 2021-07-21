//
// Created by cristobal on 7/12/21.
//

#include "OneVarIntersectBGPOp.hpp"
template <BGPOp::VARS WV>
BGPOp::RunResult
OneVarIntersectBGPOp<WV>::run(std::vector<unsigned long> &row_to_fill) {
  BGPOp::RunResult result;
  if (!time_control.tick())
    return result;

  result.scan_done = true;

  auto intersect_value = row_to_fill[var_pos];
  auto band_value = scanner->get_band_value();
  auto &k2tree = scanner->get_tree();

  if constexpr (WV == BGPOp::VARS::SUBJECT_VAR) {
    result.valid_value = k2tree.has(intersect_value, band_value);
  } else {
    result.valid_value = k2tree.has(band_value, intersect_value);
  }
  return result;
}

template <BGPOp::VARS WV> void OneVarIntersectBGPOp<WV>::reset_op() {}
template <BGPOp::VARS WV>
OneVarIntersectBGPOp<WV>::OneVarIntersectBGPOp(
    std::unique_ptr<K2TreeMixed::K2TreeScanner> &&scanner,
    unsigned long var_pos, TimeControl &time_control)
    : scanner(std::move(scanner)), var_pos(var_pos),
      time_control(time_control) {}
template <BGPOp::VARS WV>
K2TreeMixed::K2TreeScanner &OneVarIntersectBGPOp<WV>::get_scanner() {
  return *scanner;
}

template class OneVarIntersectBGPOp<BGPOp::SUBJECT_VAR>;
template class OneVarIntersectBGPOp<BGPOp::OBJECT_VAR>;
