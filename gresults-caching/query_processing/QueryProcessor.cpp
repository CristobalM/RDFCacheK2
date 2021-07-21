

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

//#include "BGPProcessor.hpp"
#include "BGPProcessor.hpp"
#include "InnerJoinProcessor.hpp"
#include "OptionalProcessor.hpp"
#include "OrderNodeProcessorVec.hpp"
#include "QueryProcessor.hpp"
#include "QueryResultIterator.hpp"
#include "ResultTableFilterIterator.hpp"
#include "ResultTableIteratorEmpty.hpp"
#include "ResultTableIteratorExtend.hpp"
#include "ResultTableIteratorFromMaterialized.hpp"
#include "ResultTableIteratorFromMaterializedVector.hpp"
#include "ResultTableIteratorMinus.hpp"
#include "ResultTableIteratorProject.hpp"
#include "ResultTableIteratorSlice.hpp"
#include "ResultTableIteratorUnion.hpp"
#include "Term.hpp"
#include "VarIndexManager.hpp"
#include "VarLazyBinding.hpp"
#include <query_processing/utility/StringHandlingUtil.hpp>
#include <quicksort_stoppable.hpp>
#include <stdexcept>

QueryProcessor::QueryProcessor(
    std::shared_ptr<PredicatesCacheManager> cache_manager,
    TimeControl &time_control)
    : QueryProcessor(std::move(cache_manager),
                     std::make_unique<VarIndexManager>(), nullptr,
                     time_control) {}

std::shared_ptr<ResultTableIterator> QueryProcessor::process_left_join_node(
    const proto_msg::LeftJoinNode &left_join_node) {
  auto left_it = process_node(left_join_node.left_node());
  if (!time_control.tick())
    return left_it;
  auto right_it = process_node(left_join_node.right_node());
  if (!time_control.tick())
    return left_it;
  auto resulting_it =
      OptionalProcessor(left_it, right_it, time_control).execute_it();
  if (!time_control.tick())
    return resulting_it;
  std::vector<const proto_msg::ExprNode *> nodes;

  for (int i = 0; i < left_join_node.expr_list_size(); i++) {
    const auto &expr_node = left_join_node.expr_list(i);
    nodes.push_back(&expr_node);
  }

  if (extra_str_dict)
    extra_str_dict = std::make_unique<NaiveDynamicStringDictionary>();

  return std::make_shared<ResultTableFilterIterator>(
      resulting_it, *vim, nodes, cm, *extra_str_dict, time_control);
}

std::shared_ptr<ResultTableIterator> QueryProcessor::process_project_node(
    const proto_msg::ProjectNode &project_node) {
  std::vector<std::string> vars;
  for (int i = 0; i < project_node.vars_size(); i++) {
    vars.push_back(project_node.vars(i));
  }

  for (const auto &var : vars) {
    vim->assign_index_if_not_found(var);
  }

  auto resulting_it = process_node(project_node.sub_op());
  if (!time_control.tick())
    return resulting_it;

  auto var_ids = get_var_ids(vars);
  return std::make_shared<ResultTableIteratorProject>(std::move(resulting_it),
                                                      var_ids, time_control);
}

std::shared_ptr<ResultTableIterator>
QueryProcessor::process_node(const proto_msg::SparqlNode &node) {

  switch (node.node_case()) {
  case proto_msg::SparqlNode::NodeCase::kProjectNode:
    return process_project_node(node.project_node());
  case proto_msg::SparqlNode::NodeCase::kBgpNode:
    return BGPProcessor(node.bgp_node(), *cm, *vim, time_control).execute_it();
  case proto_msg::SparqlNode::NodeCase::kLeftJoinNode:
    return process_left_join_node(node.left_join_node());
  case proto_msg::SparqlNode::NodeCase::kUnionNode:
    return process_union_node(node.union_node());
  case proto_msg::SparqlNode::NodeCase::kDistinctNode:
    return process_distinct_node(node.distinct_node());
  case proto_msg::SparqlNode::NodeCase::kOptionalNode:
    return process_optional_node(node.optional_node());
  case proto_msg::SparqlNode::NodeCase::kFilterNode:
    return process_filter_node(node.filter_node());
  case proto_msg::SparqlNode::NodeCase::kExtendNode:
    return process_extend_node(node.extend_node());
  case proto_msg::SparqlNode::kMinusNode:
    return process_minus_node(node.minus_node());
  case proto_msg::SparqlNode::kSequenceNode:
    return process_sequence_node(node.sequence_node());
  case proto_msg::SparqlNode::kSliceNode:
    return process_slice_node(node.slice_node());
  case proto_msg::SparqlNode::kOrderNode:
    return process_order_node(node.order_node());
  case proto_msg::SparqlNode::kTableNode:
    return process_table_node(node.table_node());
  default:
    throw std::runtime_error("Unsupported nodetype on process_node: " +
                             std::to_string(node.node_case()));
  }
}

QueryResultIterator
QueryProcessor::run_query(proto_msg::SparqlTree const &query_tree) {
  auto result = process_node(query_tree.root());
  if (extra_str_dict)
    return QueryResultIterator(result, cm, std::move(vim),
                               std::move(extra_str_dict));
  return QueryResultIterator(result, cm, std::move(vim));
}

std::shared_ptr<ResultTableIterator>
QueryProcessor::process_union_node(const proto_msg::UnionNode &union_node) {
  //  UnionProcessor union_processor;
  //  for (int i = 0; i < union_node.nodes_list_size(); i++) {
  //    auto &node = union_node.nodes_list(i);
  //    auto table = process_node(node);
  //    union_processor.combine_table(std::move(table));
  //  }
  //  return union_processor.get_result();

  std::vector<std::shared_ptr<ResultTableIterator>> its;
  for (int i = 0; i < union_node.nodes_list_size(); i++) {
    auto &node = union_node.nodes_list(i);
    auto it = process_node(node);
    if (!time_control.tick())
      return it;
    its.push_back(std::move(it));
  }

  return std::make_shared<ResultTableIteratorUnion>(std::move(its),
                                                    time_control);
}

std::shared_ptr<ResultTableIterator> QueryProcessor::process_distinct_node(
    const proto_msg::DistinctNode &distinct_node) {
  auto resulting_it = process_node(distinct_node.sub_node());
  if (!time_control.tick())
    return resulting_it;
  auto table = resulting_it->materialize_vector();
  if (!time_control.tick())
    return std::make_shared<ResultTableIteratorEmpty>(
        resulting_it->get_headers(), time_control);
  left_to_right_sort_vec(*table);
  auto result_table_list = convert_to_result_table_list(*table);
  remove_repeated_rows(*result_table_list);
  return std::make_shared<ResultTableIteratorFromMaterialized>(
      std::move(result_table_list), time_control);
}

std::shared_ptr<ResultTableIterator> QueryProcessor::process_optional_node(
    const proto_msg::OptionalNode &optional_node) {
  auto left_it = process_node(optional_node.left_node());
  if (!time_control.tick())
    return left_it;
  auto right_it = process_node(optional_node.right_node());
  if (!time_control.tick())
    return left_it;
  return OptionalProcessor(left_it, right_it, time_control).execute_it();
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

void QueryProcessor::left_to_right_sort(ResultTable &input_table) {

  input_table.data.sort([](const std::vector<unsigned long> &lhs,
                           const std::vector<unsigned long> &rhs) {
    for (size_t i = 0; i < lhs.size(); i++) {
      auto lhs_i = lhs.at(i);
      auto rhs_i = rhs.at(i);
      if (lhs_i == rhs_i)
        continue;
      return lhs_i < rhs_i;
    }
    return true;
  });
}

std::shared_ptr<ResultTableIterator>
QueryProcessor::process_filter_node(const proto_msg::FilterNode &node) {

  auto resulting_it = process_node(node.node());
  if (!time_control.tick())
    return resulting_it;
  std::vector<const proto_msg::ExprNode *> nodes;

  for (int i = 0; i < node.exprs_size(); i++) {
    const auto &expr_node = node.exprs(i);
    nodes.push_back(&expr_node);
  }

  if (!extra_str_dict)
    extra_str_dict = std::make_unique<NaiveDynamicStringDictionary>();

  //  ExprListProcessor(*resulting_it, *vim, nodes, cm, *extra_str_dict)
  //      .execute();

  return std::make_shared<ResultTableFilterIterator>(
      resulting_it, *vim, nodes, cm, *extra_str_dict, time_control);
}
std::shared_ptr<ResultTableIterator>
QueryProcessor::process_extend_node(const proto_msg::ExtendNode &node) {
  auto resulting_it = process_node(node.node());
  if (!time_control.tick())
    return resulting_it;
  if (!extra_str_dict) {
    extra_str_dict = std::make_unique<NaiveDynamicStringDictionary>();
  }

  auto var_pos_mapping = create_var_pos_mapping(*resulting_it);

  auto eval_data = std::make_unique<EvalData>(
      *vim, cm, std::move(var_pos_mapping), *extra_str_dict, time_control);
  std::vector<std::unique_ptr<VarLazyBinding>> var_bindings;
  for (int i = 0; i < node.assignments_size(); i++) {
    const auto &assignment_node = node.assignments(i);
    auto var_value = assignment_node.var().term_value();
    vim->assign_index_if_not_found(var_value);
    auto var_id = vim->var_indexes[var_value];
    // resulting_it->headers.push_back(new_header);
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
std::shared_ptr<ResultTableIterator>
QueryProcessor::process_minus_node(const proto_msg::MinusNode &node) {
  auto left_it = process_node(node.left_node());
  if (!time_control.tick())
    return left_it;
  auto right_it = process_node(node.right_node());
  if (!time_control.tick())
    return left_it;
  std::set<unsigned long> left_headers_set(left_it->get_headers().begin(),
                                           left_it->get_headers().end());
  // std::set<unsigned long> right_headers(right_it->get_headers().begin(),
  // right_it->get_headers().end());

  for (auto rh : right_it->get_headers()) {
    // right headers have to be a subset of left headers
    if (left_headers_set.find(rh) == left_headers_set.end()) {
      return left_it;
    }
  }

  return std::make_shared<ResultTableIteratorMinus>(left_it, *right_it,
                                                    time_control);
}
std::shared_ptr<ResultTableIterator>
QueryProcessor::process_sequence_node(const proto_msg::SequenceNode &node) {
  auto result_it = process_node(node.nodes(0));
  if (!time_control.tick())
    return result_it;
  for (int i = 1; i < node.nodes_size(); i++) {
    auto current_it = process_node(node.nodes(i));
    if (!time_control.tick())
      return result_it;
    result_it =
        InnerJoinProcessor(result_it, current_it, time_control).execute_it();
  }
  return result_it;
}

std::shared_ptr<ResultTableIterator>
QueryProcessor::process_slice_node(const proto_msg::SliceNode &node) {
  auto result_it = process_node(node.node());
  if (!time_control.tick())
    return result_it;
  const long start = node.start();
  const long length = node.length();

  if (start < 0 && length < 0)
    return result_it;

  return std::make_shared<ResultTableIteratorSlice>(std::move(result_it), start,
                                                    length, time_control);
}
QueryProcessor::QueryProcessor(
    std::shared_ptr<PredicatesCacheManager> cache_manager,
    std::unique_ptr<VarIndexManager> &&vim,
    std::unique_ptr<NaiveDynamicStringDictionary> &&extra_str_dict,
    TimeControl &time_control)
    : cm(std::move(cache_manager)), vim(std::move(vim)),
      extra_str_dict(std::move(extra_str_dict)), time_control(time_control) {}

std::shared_ptr<ResultTableIterator>
QueryProcessor::process_order_node(const proto_msg::OrderNode &node) {
  auto result_it = process_node(node.node());
  if (!time_control.tick())
    return result_it;
  auto var_pos_mapping = create_var_pos_mapping(*result_it);

  EvalData eval_data(*vim, cm, std::move(var_pos_mapping), *extra_str_dict,
                     time_control);

  auto table = result_it->materialize_vector();
  if (!time_control.tick())
    return std::make_shared<ResultTableIteratorEmpty>(result_it->get_headers(),
                                                      time_control);
  return std::make_shared<ResultTableIteratorFromMaterializedVector>(
      OrderNodeProcessorVec(std::move(table), node, eval_data, time_control)
          .execute(),
      time_control);
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
std::shared_ptr<ResultTable> QueryProcessor::convert_to_result_table_list(
    ResultTableVector &result_table_vector) {
  auto result = std::make_shared<ResultTable>();
  result->headers = std::move(result_table_vector.headers);

  long pos = result_table_vector.data.size() - 1;

  for (; pos >= 0; pos--) {
    if (!time_control.tick())
      return result;
    result->data.push_front(std::move(result_table_vector.data[pos]));
    result_table_vector.data.erase(result_table_vector.data.begin() + pos);
  }
  return result;
}
std::shared_ptr<ResultTableIterator>
QueryProcessor::process_table_node(const proto_msg::TableNode &node) {
  auto result_table_list = std::make_shared<ResultTable>();

  for (int i = 0; i < node.vars_size(); i++) {
    auto &var = node.vars(i);
    auto id = vim->assign_index_if_not_found(var);
    result_table_list->headers.push_back(id);
  }

  if (!extra_str_dict) {
    extra_str_dict = std::make_unique<NaiveDynamicStringDictionary>();
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
