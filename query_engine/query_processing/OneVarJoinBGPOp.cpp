//
// Created by cristobal on 7/12/21.
//

#include "OneVarJoinBGPOp.hpp"
template <BGPOp::VARS WV>
BGPOp::RunResult
OneVarJoinBGPOp<WV>::run(std::vector<unsigned long> &row_to_fill) {
  BGPOp::RunResult result;
  if (!time_control.tick())
    return result;

  if (!scanner->has_next()) {
    result.scan_done = true;
    result.valid_value = false;
    return result;
  };

  auto intersect_value = row_to_fill[var_pos];
  while (scanner->has_next()) {
    auto next_pair = scanner->next();
    if (!time_control.tick())
      return result;
    unsigned long cmp_value;
    if constexpr (WV == BGPOp::SUBJECT_VAR) {
      cmp_value = next_pair.first;
    } else {
      cmp_value = next_pair.second;
    }

    if (cmp_value == intersect_value) {
      result.valid_value = true;
      result.scan_done = !scanner->has_next();
      return result;
    }
  }

  result.scan_done = true;
  result.valid_value = false;
  return result;
}

template <BGPOp::VARS WV> void OneVarJoinBGPOp<WV>::reset_op() {
  scanner->reset_scan();
}
template <BGPOp::VARS WV>
OneVarJoinBGPOp<WV>::OneVarJoinBGPOp(std::unique_ptr<K2TreeScanner> &&scanner,
                                     unsigned long var_pos,
                                     TimeControl &time_control)
    : scanner(std::move(scanner)), var_pos(var_pos),
      time_control(time_control) {}
template <BGPOp::VARS WV> K2TreeScanner &OneVarJoinBGPOp<WV>::get_scanner() {
  return *scanner;
}

template class OneVarJoinBGPOp<BGPOp::VARS::SUBJECT_VAR>;
template class OneVarJoinBGPOp<BGPOp::VARS::OBJECT_VAR>;