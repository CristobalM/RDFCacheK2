//
// Created by cristobal on 7/12/21.
//

#include "TripleExistenceBGPOP.hpp"
TripleExistenceBGPOP::TripleExistenceBGPOP(
    std::unique_ptr<K2TreeMixed::K2TreeScanner> &&scanner,
    std::shared_ptr<Triple> triple, TimeControl &time_control)
    : scanner(std::move(scanner)), triple(std::move(triple)),
      time_control(time_control) {}
BGPOp::RunResult TripleExistenceBGPOP::run(std::vector<unsigned long> &) {
  BGPOp::RunResult result;
  if (!time_control.tick())
    return result;

  auto &k2tree = scanner->get_tree();
  result.valid_value =
      k2tree.has(triple->subject.id_value, triple->object.id_value);
  result.scan_done = true;
  return result;
}
void TripleExistenceBGPOP::reset_op() {}
K2TreeMixed::K2TreeScanner &TripleExistenceBGPOP::get_scanner() {
  return *scanner;
}
