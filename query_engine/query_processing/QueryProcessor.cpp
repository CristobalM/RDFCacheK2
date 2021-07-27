

//
// Created by Cristobal Miranda, 2020
//

#include <TimeControl.hpp>
#include <chrono>
#include <query_processing/expr/ExprEval.hpp>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include "BGPProcessor.hpp"
#include "MinusProcessor.hpp"
#include "OptionalProcessor.hpp"
#include "QueryProcessor.hpp"
#include "QueryResultIterator.hpp"
#include "ResultTableFilterIterator.hpp"
#include "ResultTableIteratorEmpty.hpp"
#include "ResultTableIteratorExtend.hpp"
#include "ResultTableIteratorFromMaterialized.hpp"
#include "ResultTableIteratorOrder.hpp"
#include "ResultTableIteratorProject.hpp"
#include "ResultTableIteratorSlice.hpp"
#include "ResultTableIteratorUnion.hpp"
#include "SequenceProcessor.hpp"
#include "Term.hpp"
#include "VarBindingQProc.hpp"
#include "VarIndexManager.hpp"
#include "VarLazyBinding.hpp"
#include <query_processing/utility/StringHandlingUtil.hpp>
#include <quicksort_stoppable.hpp>

QueryProcessor::QueryProcessor(
    std::shared_ptr<PredicatesCacheManager> cache_manager,
    TimeControl &time_control, const std::string &temp_files_dir)
    : QueryProcessor(std::move(cache_manager),
                     std::make_unique<VarIndexManager>(),
                     std::make_shared<NaiveDynamicStringDictionary>(),
                     time_control, temp_files_dir) {}

std::shared_ptr<ResultTableIterator> QueryProcessor::process_left_join_node(
    const proto_msg::LeftJoinNode &left_join_node,
    const std::shared_ptr<VarBindingQProc> &var_binding_qproc) {
  auto resulting_it =
      OptionalProcessor(left_join_node.left_node(), left_join_node.right_node(),
                        this, var_binding_qproc, time_control)
          .execute_it();
  if (!time_control.tick())
    return resulting_it;
  std::vector<const proto_msg::ExprNode *> nodes;

  for (int i = 0; i < left_join_node.expr_list_size(); i++) {
    const auto &expr_node = left_join_node.expr_list(i);
    nodes.push_back(&expr_node);
  }

  return std::make_shared<ResultTableFilterIterator>(
      resulting_it, *vim, nodes, cm, extra_str_dict, time_control,
      var_binding_qproc, temp_files_dir);
}

std::shared_ptr<ResultTableIterator> QueryProcessor::process_project_node(
    const proto_msg::ProjectNode &project_node,
    std::shared_ptr<VarBindingQProc> var_binding_qproc) {
  std::vector<std::string> vars;
  vars.reserve(project_node.vars_size());
  for (int i = 0; i < project_node.vars_size(); i++) {
    vars.push_back(project_node.vars(i));
  }

  for (const auto &var : vars) {
    vim->assign_index_if_not_found(var);
  }

  auto resulting_it =
      process_node(project_node.sub_op(), std::move(var_binding_qproc));
  if (!time_control.tick())
    return resulting_it;

  auto var_ids = get_var_ids(vars);
  return std::make_shared<ResultTableIteratorProject>(std::move(resulting_it),
                                                      var_ids, time_control);
}

std::shared_ptr<ResultTableIterator> QueryProcessor::process_node(
    const proto_msg::SparqlNode &node,
    std::shared_ptr<VarBindingQProc> var_binding_qproc) {

  switch (node.node_case()) {
  case proto_msg::SparqlNode::NodeCase::kProjectNode:
    return process_project_node(node.project_node(), var_binding_qproc);
  case proto_msg::SparqlNode::NodeCase::kBgpNode:
    return BGPProcessor(node.bgp_node(), *cm, *vim, time_control,
                        var_binding_qproc)
        .execute_it();
  case proto_msg::SparqlNode::NodeCase::kLeftJoinNode:
    return process_left_join_node(node.left_join_node(), var_binding_qproc);
  case proto_msg::SparqlNode::NodeCase::kUnionNode:
    return process_union_node(node.union_node(), var_binding_qproc);
  case proto_msg::SparqlNode::NodeCase::kDistinctNode:
    return process_distinct_node(node.distinct_node(), var_binding_qproc);
  case proto_msg::SparqlNode::NodeCase::kOptionalNode:
    return process_optional_node(node.optional_node(), var_binding_qproc);
  case proto_msg::SparqlNode::NodeCase::kFilterNode:
    return process_filter_node(node.filter_node(), var_binding_qproc);
  case proto_msg::SparqlNode::NodeCase::kExtendNode:
    return process_extend_node(node.extend_node(), var_binding_qproc);
  case proto_msg::SparqlNode::kMinusNode:
    return process_minus_node(node.minus_node(), var_binding_qproc);
  case proto_msg::SparqlNode::kSequenceNode:
    return process_sequence_node(node.sequence_node(), var_binding_qproc);
  case proto_msg::SparqlNode::kSliceNode:
    return process_slice_node(node.slice_node(), var_binding_qproc);
  case proto_msg::SparqlNode::kOrderNode:
    return process_order_node(node.order_node(), var_binding_qproc);
  case proto_msg::SparqlNode::kTableNode:
    return process_table_node(node.table_node(), var_binding_qproc);
  default:
    throw std::runtime_error("Unsupported nodetype on process_node: " +
                             std::to_string(node.node_case()));
  }
}

QueryResultIterator
QueryProcessor::run_query(const proto_msg::SparqlNode &query_tree) {
  auto var_binding_qproc = std::make_shared<VarBindingQProc>();
  auto result = process_node(query_tree, var_binding_qproc);
  if (extra_str_dict)
    return QueryResultIterator(result, cm, vim, std::move(extra_str_dict));
  return QueryResultIterator(result, cm, vim);
}

std::shared_ptr<ResultTableIterator> QueryProcessor::process_union_node(
    const proto_msg::UnionNode &union_node,
    const std::shared_ptr<VarBindingQProc> &var_binding_qproc) {
  std::vector<std::shared_ptr<ResultTableIterator>> its;
  for (int i = 0; i < union_node.nodes_list_size(); i++) {
    auto &node = union_node.nodes_list(i);
    auto it = process_node(node, var_binding_qproc);
    if (!time_control.tick())
      return std::make_shared<ResultTableIteratorEmpty>(time_control);
    its.push_back(std::move(it));
  }

  return std::make_shared<ResultTableIteratorUnion>(std::move(its),
                                                    time_control);
}

std::shared_ptr<ResultTableIterator> QueryProcessor::process_distinct_node(
    const proto_msg::DistinctNode &distinct_node,
    std::shared_ptr<VarBindingQProc> var_binding_qproc) {

  auto result_it = process_node(distinct_node.sub_node(), var_binding_qproc);
  if (!time_control.tick())
    return std::make_shared<ResultTableIteratorEmpty>(time_control);

  proto_msg::OrderNode order_node;
  auto &headers = result_it->get_headers();
  auto reversed_vim = vim->reverse();
  for (auto value : headers) {
    auto s_header = reversed_vim[value];
    auto *sort_condition = order_node.mutable_sort_conditions()->Add();
    auto *term_node = sort_condition->mutable_expr()->mutable_term_node();
    term_node->set_term_value(s_header);
    term_node->set_term_type(proto_msg::VARIABLE);
    sort_condition->set_direction(proto_msg::SortDirection::ASCENDING);
  }

  auto var_pos_mapping = create_var_pos_mapping(*result_it);

  EvalData eval_data(*vim, cm, std::move(var_pos_mapping), extra_str_dict,
                     time_control, var_binding_qproc, temp_files_dir);

  return std::make_shared<ResultTableIteratorOrder>(
      std::move(result_it), order_node, std::move(eval_data), true,
      time_control);
}

std::shared_ptr<ResultTableIterator> QueryProcessor::process_optional_node(
    const proto_msg::OptionalNode &optional_node,
    const std::shared_ptr<VarBindingQProc> &var_binding_qproc) {
  return OptionalProcessor(optional_node.left_node(),
                           optional_node.right_node(), this, var_binding_qproc,
                           time_control)
      .execute_it();
}

void QueryProcessor::remove_repeated_rows(ResultTable &input_table) {
  auto left_it = input_table.data.begin();

  for (auto it = std::next(left_it); it != input_table.data.end();) {
    if (!time_control.tick())
      return;
    if (*it == *left_it) {
      auto next_it = std::next(it);
      input_table.data.erase(it);
      it = next_it;
    } else {
      left_it = it;
      it++;
    }
  }
}

std::shared_ptr<ResultTableIterator> QueryProcessor::process_filter_node(
    const proto_msg::FilterNode &node,
    const std::shared_ptr<VarBindingQProc> &var_binding_qproc) {

  auto resulting_it = process_node(node.node(), var_binding_qproc);
  if (!time_control.tick())
    return std::make_shared<ResultTableIteratorEmpty>(time_control);
  std::vector<const proto_msg::ExprNode *> nodes;

  for (int i = 0; i < node.exprs_size(); i++) {
    const auto &expr_node = node.exprs(i);
    nodes.push_back(&expr_node);
  }

  return std::make_shared<ResultTableFilterIterator>(
      resulting_it, *vim, nodes, cm, extra_str_dict, time_control,
      var_binding_qproc, temp_files_dir);
}
std::shared_ptr<ResultTableIterator> QueryProcessor::process_extend_node(
    const proto_msg::ExtendNode &node,
    const std::shared_ptr<VarBindingQProc> &var_binding_qproc) {
  auto resulting_it = process_node(node.node(), var_binding_qproc);
  if (!time_control.tick())
    return std::make_shared<ResultTableIteratorEmpty>(time_control);

  auto var_pos_mapping = create_var_pos_mapping(*resulting_it);

  auto eval_data = std::make_unique<EvalData>(
      *vim, cm, std::move(var_pos_mapping), extra_str_dict, time_control,
      var_binding_qproc, temp_files_dir);
  std::vector<std::unique_ptr<VarLazyBinding>> var_bindings;
  for (int i = 0; i < node.assignments_size(); i++) {
    const auto &assignment_node = node.assignments(i);
    auto var_value = assignment_node.var().term_value();
    vim->assign_index_if_not_found(var_value);
    auto var_id = vim->var_indexes[var_value];
    auto expr_eval =
        ExprEval::create_eval_node(*eval_data, assignment_node.expr());
    expr_eval->init();
    var_bindings.push_back(
        std::make_unique<VarLazyBinding>(var_id, std::move(expr_eval)));
  }
  return std::make_shared<ResultTableIteratorExtend>(
      std::move(resulting_it), std::move(eval_data), std::move(var_bindings),
      time_control);
}
std::shared_ptr<ResultTableIterator> QueryProcessor::process_minus_node(
    const proto_msg::MinusNode &node,
    const std::shared_ptr<VarBindingQProc> &var_binding_qproc) {
  // auto left_it = process_node(node.left_node(), var_binding_qproc);
  return MinusProcessor(node, this, time_control, var_binding_qproc)
      .execute_it();
}
std::shared_ptr<ResultTableIterator> QueryProcessor::process_sequence_node(
    const proto_msg::SequenceNode &node,
    std::shared_ptr<VarBindingQProc> var_binding_qproc) {
  return SequenceProcessor(node, this, time_control,
                           std::move(var_binding_qproc))
      .execute_it();
  //
  //  auto result_it = process_node(node.nodes(0));
  //  if (!time_control.tick())
  //    return result_it;
  //  for (int i = 1; i < node.nodes_size(); i++) {
  //    auto current_it = process_node(node.nodes(i));
  //    if (!time_control.tick())
  //      return result_it;
  //    result_it =
  //        InnerJoinProcessor(result_it, current_it,
  //        time_control).execute_it();
  //  }
  //  return result_it;
}

std::shared_ptr<ResultTableIterator> QueryProcessor::process_slice_node(
    const proto_msg::SliceNode &node,
    std::shared_ptr<VarBindingQProc> var_binding_qproc) {
  auto result_it = process_node(node.node(), std::move(var_binding_qproc));
  if (!time_control.tick())
    return std::make_shared<ResultTableIteratorEmpty>(time_control);
  const long start = node.start();
  const long length = node.length();

  if (start < 0 && length < 0)
    return result_it;

  return std::make_shared<ResultTableIteratorSlice>(std::move(result_it), start,
                                                    length, time_control);
}
QueryProcessor::QueryProcessor(
    std::shared_ptr<PredicatesCacheManager> cache_manager,
    std::shared_ptr<VarIndexManager> vim,
    std::shared_ptr<NaiveDynamicStringDictionary> extra_str_dict,
    TimeControl &time_control, const std::string &temp_files_dir)
    : cm(std::move(cache_manager)), vim(std::move(vim)),
      extra_str_dict(std::move(extra_str_dict)), time_control(time_control),
      temp_files_dir(temp_files_dir) {}

std::shared_ptr<ResultTableIterator> QueryProcessor::process_order_node(
    const proto_msg::OrderNode &node,
    const std::shared_ptr<VarBindingQProc> &var_binding_qproc) {
  auto result_it = process_node(node.node(), var_binding_qproc);
  if (!time_control.tick())
    return std::make_shared<ResultTableIteratorEmpty>(time_control);
  auto var_pos_mapping = create_var_pos_mapping(*result_it);

  EvalData eval_data(*vim, cm, std::move(var_pos_mapping), extra_str_dict,
                     time_control, var_binding_qproc, temp_files_dir);

  return std::make_shared<ResultTableIteratorOrder>(
      std::move(result_it), node, std::move(eval_data), false, time_control);
}

std::shared_ptr<std::unordered_map<std::string, unsigned long>>
QueryProcessor::create_var_pos_mapping(ResultTableIterator &input_it) {
  auto var_pos_mapping =
      std::make_shared<std::unordered_map<std::string, unsigned long>>();
  auto rev_map = vim->reverse();
  auto &headers = input_it.get_headers();
  for (unsigned long i = 0; i < static_cast<unsigned long>(headers.size());
       i++) {
    auto header = headers[i];
    (*var_pos_mapping)[rev_map[header]] = i;
  }
  return var_pos_mapping;
}
std::set<unsigned long>
QueryProcessor::get_var_ids(const std::vector<std::string> &vars_vector) {
  std::set<unsigned long> result;
  for (auto &v : vars_vector) {
    result.insert(vim->var_indexes[v]);
  }
  return result;
}
void QueryProcessor::left_to_right_sort_vec(
    ResultTableVector &result_table_vector) {

  inplace_quicksort_stoppable(
      result_table_vector.data,
      [](const std::vector<unsigned long> &lhs,
         const std::vector<unsigned long> &rhs) {
        for (size_t i = 0; i < lhs.size(); i++) {
          auto lhs_i = lhs.at(i);
          auto rhs_i = rhs.at(i);
          if (lhs_i == rhs_i)
            continue;
          return lhs_i < rhs_i;
        }
        return true;
      },
      time_control);
}
std::shared_ptr<ResultTableIterator>
QueryProcessor::process_table_node(const proto_msg::TableNode &node,
                                   const std::shared_ptr<VarBindingQProc> &) {
  auto result_table_list = std::make_shared<ResultTable>();

  for (int i = 0; i < node.vars_size(); i++) {
    auto &var = node.vars(i);
    auto id = vim->assign_index_if_not_found(var);
    result_table_list->headers.push_back(id);
  }

  for (int i = 0; i < node.rows_size(); i++) {
    std::vector<unsigned long> row(node.vars_size(), 0);
    const auto &proto_row = node.rows(i);
    for (int j = 0; j < proto_row.values_size(); j++) {
      const auto &value = proto_row.values(j);
      auto resource = resource_from_proto_term(value);
      auto id = id_from_resource(resource);
      if (!id) {
        extra_str_dict->add_resource(resource);
        id = id_from_resource(resource);
      }
      row[j] = id;
    }
    result_table_list->data.push_back(row);
  }
  return std::make_shared<ResultTableIteratorFromMaterialized>(
      std::move(result_table_list), time_control);
}

RDFResource
QueryProcessor::resource_from_proto_term(const proto_msg::RDFTerm &term) {
  RDFResourceType res_type = resource_type_from_term(term);
  return RDFResource(term.term_value(), res_type);
}
RDFResourceType
QueryProcessor::resource_type_from_term(const proto_msg::RDFTerm &term) {
  switch (term.term_type()) {
  case proto_msg::IRI:
    return RDFResourceType::RDF_TYPE_IRI;
  case proto_msg::LITERAL:
    return RDFResourceType::RDF_TYPE_LITERAL;
  case proto_msg::BLANK_NODE:
    return RDFResourceType::RDF_TYPE_BLANK;
  default:
    throw std::runtime_error(
        "Can't translate proto_msg::TermType to RDFResource");
  }
}
unsigned long QueryProcessor::id_from_resource(RDFResource &resource) {
  auto id = cm->get_resource_index(resource);
  if (!id && extra_str_dict) {
    auto extra_id = extra_str_dict->locate_resource(resource);
    if (!extra_id)
      return 0;
    auto last_cm_id = cm->get_last_id();
    return last_cm_id + extra_id;
  }
  return id;
}
VarIndexManager &QueryProcessor::get_vim() { return *vim; }
std::shared_ptr<PredicatesCacheManager> QueryProcessor::get_cache_manager() {
  return cm;
}
QueryResultIterator
QueryProcessor::run_query(const proto_msg::SparqlNode &proto_node,
                          std::shared_ptr<VarBindingQProc> var_binding_qproc) {
  auto result = process_node(proto_node, std::move(var_binding_qproc));
  if (extra_str_dict)
    return QueryResultIterator(result, cm, std::move(vim),
                               std::move(extra_str_dict));
  return QueryResultIterator(result, cm, std::move(vim));
}
std::shared_ptr<VarIndexManager> QueryProcessor::get_vim_ptr() { return vim; }
std::shared_ptr<NaiveDynamicStringDictionary>
QueryProcessor::get_extra_str_dict_ptr() {
  return extra_str_dict;
}
const std::string &QueryProcessor::get_temp_files_dir() {
  return temp_files_dir;
}
