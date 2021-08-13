//
// Created by cristobal on 7/12/21.
//

#include "BGPOpsFactory.hpp"
#include "OneVarCPBGPOp.hpp"
#include "OneVarIntersectBGPOp.hpp"
#include "TripleExistenceBGPOP.hpp"
#include "TwoVarCProductBGPOp.hpp"
#include "TwoVarIntersectionBGPOp.hpp"
#include "TwoVarJoinOneBGPOp.hpp"
std::unique_ptr<BGPOp> BGPOpsFactory::create_bgp_op(
    std::unique_ptr<K2TreeScanner> &&scanner, Triple &triple,
    VarIndexManager &vim, std::vector<long> &join_incidence,
    std::unordered_map<unsigned long, unsigned long> &header_rev_map,
    TimeControl &time_control, VarBindingQProc &var_binding_qproc) {
  unsigned long subject_var_id = 0;
  unsigned long object_var_id = 0;
  if (triple.subject.type == TermType::VAR)
    subject_var_id = vim.var_indexes[triple.subject.value];
  if (triple.object.type == TermType::VAR)
    object_var_id = vim.var_indexes[triple.object.value];

  auto subject_is_var = triple.subject.type == TermType::VAR &&
                        !var_binding_qproc.is_bound(subject_var_id);
  auto object_is_var = triple.object.type == TermType::VAR &&
                       !var_binding_qproc.is_bound(object_var_id);

  if (subject_is_var && object_is_var) {
    unsigned long subject_pos = header_rev_map[subject_var_id];
    unsigned long object_pos = header_rev_map[object_var_id];
    auto subject_incidence = join_incidence[subject_pos];
    auto object_incidence = join_incidence[object_pos];
    join_incidence[subject_pos]++;
    join_incidence[object_pos]++;
    if (subject_incidence > 0 && object_incidence > 0)
      return std::make_unique<TwoVarIntersectionBGPOp>(
          std::move(scanner), subject_pos, object_pos, time_control);
    if (subject_incidence > 0)
      return std::make_unique<TwoVarJoinOneBGPOp<BGPOp::SUBJECT_VAR>>(
          std::move(scanner), subject_pos, object_pos, time_control);
    if (object_incidence > 0)
      return std::make_unique<TwoVarJoinOneBGPOp<BGPOp::OBJECT_VAR>>(
          std::move(scanner), subject_pos, object_pos, time_control);
    // here none of the variables would be considered yet
    return std::make_unique<TwoVarCProductBGPOp>(
        std::move(scanner), subject_pos, object_pos, time_control);
  }

  if (subject_is_var) {
    unsigned long subject_pos = header_rev_map[subject_var_id];
    auto subject_incidence = join_incidence[subject_pos];
    join_incidence[subject_pos]++;
    if (subject_incidence > 0)
      return std::make_unique<OneVarIntersectBGPOp<BGPOp::SUBJECT_VAR>>(
          std::move(scanner), subject_pos, time_control);
    return std::make_unique<OneVarCPBGPOp<BGPOp::SUBJECT_VAR>>(
        std::move(scanner), subject_pos, time_control);
  }

  if (object_is_var) {
    unsigned long object_pos = header_rev_map[object_var_id];
    auto object_incidence = join_incidence[object_pos];
    join_incidence[object_pos]++;
    if (object_incidence > 0)
      return std::make_unique<OneVarIntersectBGPOp<BGPOp::OBJECT_VAR>>(
          std::move(scanner), object_pos, time_control);
    return std::make_unique<OneVarCPBGPOp<BGPOp::OBJECT_VAR>>(
        std::move(scanner), object_pos, time_control);
  }

  // reaching this point means that there are no vars in the triple

  unsigned long subject_value_id = 0;
  unsigned long object_value_id = 0;

  if (triple.subject.type == TermType::VAR)
    subject_value_id = var_binding_qproc.get_value(subject_var_id);
  else
    subject_value_id = triple.subject.id_value;

  if (triple.object.type == TermType::VAR)
    object_value_id = var_binding_qproc.get_value(object_var_id);
  else
    object_value_id = triple.object.id_value;

  return std::make_unique<TripleExistenceBGPOP>(
      std::move(scanner), subject_value_id, object_value_id, time_control);
}
