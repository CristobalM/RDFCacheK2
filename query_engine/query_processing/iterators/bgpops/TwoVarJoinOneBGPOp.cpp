//
// Created by cristobal on 7/12/21.
//

#include "TwoVarJoinOneBGPOp.hpp"

template <BGPOp::VARS WV>
TwoVarJoinOneBGPOp<WV>::TwoVarJoinOneBGPOp(
    std::unique_ptr<K2TreeScanner> &&scanner, unsigned long subject_pos,
    unsigned long object_pos, TimeControl &time_control)
    : scanner(std::move(scanner)), subject_pos(subject_pos),
      object_pos(object_pos), current_band(0), time_control(time_control) {}
template <BGPOp::VARS WV>
BGPOp::RunResult
TwoVarJoinOneBGPOp<WV>::run(std::vector<unsigned long> &row_to_fill) {
  BGPOp::RunResult result;
  if (!time_control.tick())
    return result;

  auto &k2tree = scanner->get_tree();
  unsigned long join_value;
  unsigned long *set_value;
  K2TreeScanner::BandType band_type;
  if constexpr (WV == BGPOp::SUBJECT_VAR) {
    join_value = row_to_fill[subject_pos];
    set_value = &row_to_fill[object_pos];
    band_type = K2TreeScanner::BandType::COLUMN_BAND_TYPE;
  } else {
    join_value = row_to_fill[object_pos];
    set_value = &row_to_fill[subject_pos];
    band_type = K2TreeScanner::BandType::ROW_BAND_TYPE;
  }

  if (!current_band_scanner || join_value != current_band) {
    current_band_scanner = k2tree.create_band_scanner(join_value, band_type);
    current_band = join_value;
    current_band_type = band_type;
  }

  if (!current_band_scanner->has_next()) {
    result.scan_done = true;
    result.valid_value = false;
    return result;
  }

  auto next_pair = current_band_scanner->next();
  if (!time_control.tick())
    return result;
  if constexpr (WV == BGPOp::SUBJECT_VAR) {
    *set_value = next_pair.second;
  } else {
    *set_value = next_pair.first;
  }
  result.valid_value = true;
  result.scan_done = !current_band_scanner->has_next();
  return result;
}
template <BGPOp::VARS WV> void TwoVarJoinOneBGPOp<WV>::reset_op() {
  current_band_scanner = nullptr;
  current_band = 0;
}
template <BGPOp::VARS WV> K2TreeScanner &TwoVarJoinOneBGPOp<WV>::get_scanner() {
  return *scanner;
}

template class TwoVarJoinOneBGPOp<BGPOp::VARS::SUBJECT_VAR>;
template class TwoVarJoinOneBGPOp<BGPOp::VARS::OBJECT_VAR>;