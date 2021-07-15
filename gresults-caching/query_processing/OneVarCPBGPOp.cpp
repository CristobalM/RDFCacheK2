//
// Created by cristobal on 7/12/21.
//

#include "OneVarCPBGPOp.hpp"

template <BGPOp::VARS WV>
OneVarCPBGPOp<WV>::OneVarCPBGPOp(
    std::unique_ptr<K2TreeMixed::K2TreeScanner> &&scanner,
    unsigned long var_pos)
    : scanner(std::move(scanner)), var_pos(var_pos) {}
template <BGPOp::VARS WV>
BGPOp::RunResult
OneVarCPBGPOp<WV>::run(std::vector<unsigned long> &row_to_fill) {
  BGPOp::RunResult result;
  if (!scanner->has_next()) {
    result.scan_done = true;
    result.valid_value = false;
    return result;
  };

  auto next_pair = scanner->next();

  unsigned long value;
  if constexpr (WV == BGPOp::SUBJECT_VAR) {
    value = next_pair.first;
  } else {
    value = next_pair.second;
  }

  row_to_fill[var_pos] = value;
  result.scan_done = !scanner->has_next();
  result.valid_value = true;

  return result;
};

template <BGPOp::VARS WV> void OneVarCPBGPOp<WV>::reset_op() {
  scanner->reset_scan();
}
template <BGPOp::VARS WV>
K2TreeMixed::K2TreeScanner &OneVarCPBGPOp<WV>::get_scanner() {
  return *scanner;
};

template class OneVarCPBGPOp<BGPOp::VARS::SUBJECT_VAR>;
template class OneVarCPBGPOp<BGPOp::VARS::OBJECT_VAR>;