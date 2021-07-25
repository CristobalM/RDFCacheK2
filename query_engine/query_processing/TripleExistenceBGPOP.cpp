//
// Created by cristobal on 7/12/21.
//

#include "TripleExistenceBGPOP.hpp"
TripleExistenceBGPOP::TripleExistenceBGPOP(
    std::unique_ptr<K2TreeScanner> &&scanner, unsigned long subject_id,
    unsigned long object_id, TimeControl &time_control)
    : scanner(std::move(scanner)), subject_id(subject_id), object_id(object_id),
      time_control(time_control) {}
BGPOp::RunResult TripleExistenceBGPOP::run(std::vector<unsigned long> &) {
  BGPOp::RunResult result;
  if (!time_control.tick())
    return result;

  auto &k2tree = scanner->get_tree();
  result.valid_value = k2tree.has(subject_id, object_id);
  result.scan_done = true;
  return result;
}
void TripleExistenceBGPOP::reset_op() {}
K2TreeScanner &TripleExistenceBGPOP::get_scanner() { return *scanner; }
