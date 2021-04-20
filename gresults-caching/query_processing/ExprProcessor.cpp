

#include "ExprProcessor.hpp"

#include "expr/BoundEval.hpp"
#include "expr/IsBlankEval.hpp"
#include "expr/IsIRIEval.hpp"
#include "expr/IsLiteralEval.hpp"
#include "expr/IsNumericEval.hpp"
#include "expr/LogicalNotEval.hpp"

#include <pcrecpp.h>

ExprProcessor::ExprProcessor(
    const ResultTable &table, const proto_msg::ExprNode &expr_node,
    const VarIndexManager &vim, const PredicatesCacheManager &cm,
    const std::unordered_map<std::string, unsigned long> &var_pos_mapping)
    : eval_data(table, vim, cm, var_pos_mapping), expr_node(expr_node) {}

ExprProcessor::ExprProcessor(EvalData eval_data,
                             const proto_msg::ExprNode &expr_node)
    : eval_data(std::move(eval_data)), expr_node(expr_node) {}

ExprProcessorPersistentData ExprProcessor::persistent_data =
    ExprProcessorPersistentData();

std::unique_ptr<BoolExprEval> ExprProcessor::create_evaluator() {

  if (expr_node.expr_case() != proto_msg::ExprNode::kFunctionNode)
    throw std::runtime_error("Base node should be function, not term");

  const auto &function_node = expr_node.function_node();

  switch (function_node.function_op()) {
  case proto_msg::FunctionOP::IS_LITERAL:
    return create_unary<IsLiteralEval>(function_node);
  case proto_msg::FunctionOP::BOUND:
    return create_unary<BoundEval>(function_node);
  case proto_msg::FunctionOP::IS_BLANK:
    return create_unary<IsBlankEval>(function_node);
  case proto_msg::FunctionOP::IS_IRI:
    return create_unary<IsIRIEval>(function_node);
  case proto_msg::FunctionOP::IS_NUMERIC:
    return create_unary<IsNumericEval>(function_node);
  case proto_msg::FunctionOP::LOGICAL_NOT:
    return create_unary<LogicalNotEval>(function_node);
  case proto_msg::FunctionOP::LANG_MATCHES:
    return process_lang_matches(row, function_node.exprs(0),
                                function_node.exprs(1));
  case proto_msg::FunctionOP::EQUALS:
    return process_equals(row, function_node.exprs(0), function_node.exprs(1));
  case proto_msg::FunctionOP::GREATER_THAN:
    return process_greater_than(row, function_node.exprs(0),
                                function_node.exprs(1));
  case proto_msg::FunctionOP::GREATER_THAN_OR_EQUAL:
    return process_greater_than_or_equal(row, function_node.exprs(0),
                                         function_node.exprs(1));
  case proto_msg::FunctionOP::LESS_THAN:
    return process_less_than(row, function_node.exprs(0),
                             function_node.exprs(1));
  case proto_msg::FunctionOP::LESS_THAN_OR_EQUAL:
    return process_less_than_or_equal(row, function_node.exprs(0),
                                      function_node.exprs(1));
  case proto_msg::FunctionOP::LOGICAL_AND:
    return process_logical_and(row, function_node.exprs(0),
                               function_node.exprs(1));
  case proto_msg::FunctionOP::LOGICAL_OR:
    return process_logical_or(row, function_node.exprs(0),
                              function_node.exprs(1));
  case proto_msg::FunctionOP::NOT_EQUALS:
    return process_not_equals(row, function_node.exprs(0),
                              function_node.exprs(1));
  case proto_msg::FunctionOP::SAME_TERM:
    return process_same_term(row, function_node.exprs(0),
                             function_node.exprs(1));
  case proto_msg::FunctionOP::STR_CONTAINS:
    return process_str_contains(row, function_node.exprs(0),
                                function_node.exprs(1));
  case proto_msg::FunctionOP::STR_ENDS_WITH:
    return process_str_ends_with(row, function_node.exprs(0),
                                 function_node.exprs(1));
  case proto_msg::FunctionOP::STR_LANG:
    return process_str_lang(row, function_node.exprs(0),
                            function_node.exprs(1));
  case proto_msg::FunctionOP::STR_STARTS_WITH:
    return process_str_starts_with(row, function_node.exprs(0),
                                   function_node.exprs(1));
  case proto_msg::FunctionOP::SUBSTRACT:
    return process_substract(row, function_node.exprs(0),
                             function_node.exprs(1));
  case proto_msg::FunctionOP::CONDITIONAL:
    return process_conditional(row, function_node.exprs(0),
                               function_node.exprs(1), function_node.exprs(2));
  case proto_msg::FunctionOP::BNODE:
    return process_bnode(row, function_node);
  case proto_msg::FunctionOP::CALL:
    return process_call(row, function_node);
  case proto_msg::FunctionOP::COALESCE:
    return process_coalesce(row, function_node);
  case proto_msg::FunctionOP::FUNCTION:
    return process_function(row, function_node);
  case proto_msg::FunctionOP::REGEX:
    return process_regex(row, function_node);
  case proto_msg::FunctionOP::STR_CONCAT:
    return process_str_concat(row, function_node);
  case proto_msg::FunctionOP::STR_REPLACE:
    return process_str_replace(row, function_node);
  case proto_msg::FunctionOP::STR_SUBSTRING:
    return process_str_substring(row, function_node);
  default:
    throw std::runtime_error("Unknown function op : " +
                             std::to_string(function_node.function_op()));
  }

  bool ExprProcessor::process_is_literal(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &node) const {
    if (node.expr_case() != proto_msg::ExprNode::kTermNode ||
        node.term_node().term_type() != proto_msg::TermType::VARIABLE)
      throw std::runtime_error(
          "Invalid operand for IS_LITERAL(), expected a variable");
    const auto &var = node.term_node().term_value();
    if (var_pos_mapping.find(var) == var_pos_mapping.end())
      throw std::runtime_error("Variable " + var + " not in table");
    auto pos = var_pos_mapping.at(var);
    auto value_id = row[pos];
    auto resource = cm.extract_resource(value_id);
    return resource.resource_type == RDFResourceType::RDF_TYPE_LITERAL;
  }
  bool ExprProcessor::process_bound(const ExprProcessor::row_t &row,
                                    const proto_msg::ExprNode &node) const {
    if (node.expr_case() != proto_msg::ExprNode::kTermNode ||
        node.term_node().term_type() != proto_msg::TermType::VARIABLE)
      throw std::runtime_error(
          "Invalid operand for BOUND(), expected a variable");
    const auto &var = node.term_node().term_value();
    return var_pos_mapping.find(var) != var_pos_mapping.end();
  }
  ExprDataType ExprProcessor::process_data_type(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &node) const {
    if (node.expr_case() != proto_msg::ExprNode::kTermNode ||
        node.term_node().term_type() != proto_msg::TermType::VARIABLE)
      throw std::runtime_error(
          "Invalid operand for DATATYPE(), expected a variable");
    const auto &var = node.term_node().term_value();
    if (var_pos_mapping.find(var) == var_pos_mapping.end())
      throw std::runtime_error("Variable " + var + " not in table");
    auto pos = var_pos_mapping.at(var);
    auto value_id = row[pos];
    auto resource = cm.extract_resource(value_id);
  }

  bool ExprProcessor::process_date_time_day(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &node) const {
    // should return a date time day
  }
  bool ExprProcessor::process_date_time_hours(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_date_time_month(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_date_time_tz(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_date_time_year(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_fun_iri(const ExprProcessor::row_t &row,
                                      const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_is_blank(const ExprProcessor::row_t &row,
                                       const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_is_iri(const ExprProcessor::row_t &row,
                                     const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_is_numeric(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_lang(const ExprProcessor::row_t &row,
                                   const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_logical_not(const proto_msg::ExprNode &node)
      const {}
  bool ExprProcessor::process_num_abs(const ExprProcessor::row_t &row,
                                      const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_num_ceiling(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_num_floor(const ExprProcessor::row_t &row,
                                        const proto_msg::ExprNode &node) const {
  }
  bool ExprProcessor::process_num_round(const ExprProcessor::row_t &row,
                                        const proto_msg::ExprNode &node) const {
  }
  bool ExprProcessor::process_str(const ExprProcessor::row_t &row,
                                  const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_str_encode_for_uri(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_str_length(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_str_lowercase(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_str_uppercase(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_unary_minus(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_unary_minus(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_digest_sha1(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_digest_md5(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_sha224(const ExprProcessor::row_t &row,
                                     const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_sha256(const ExprProcessor::row_t &row,
                                     const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_sha384(const ExprProcessor::row_t &row,
                                     const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_sha512(const ExprProcessor::row_t &row,
                                     const proto_msg::ExprNode &node) const {}
  bool ExprProcessor::process_lang_matches(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &first,
      const proto_msg::ExprNode &second) const {}
  bool ExprProcessor::process_add(const ExprProcessor::row_t &row,
                                  const proto_msg::ExprNode &first,
                                  const proto_msg::ExprNode &second) const {}
  bool ExprProcessor::process_cast(const ExprProcessor::row_t &row,
                                   const proto_msg::ExprNode &first,
                                   const proto_msg::ExprNode &second) const {}
  bool ExprProcessor::process_divide(const ExprProcessor::row_t &row,
                                     const proto_msg::ExprNode &first,
                                     const proto_msg::ExprNode &second) const {}
  bool ExprProcessor::process_equals(const ExprProcessor::row_t &row,
                                     const proto_msg::ExprNode &first,
                                     const proto_msg::ExprNode &second) const {}
  bool ExprProcessor::process_greater_than(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &first,
      const proto_msg::ExprNode &second) const {}
  bool ExprProcessor::process_greater_than_or_equal(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &first,
      const proto_msg::ExprNode &second) const {}
  bool ExprProcessor::process_less_than(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &first,
      const proto_msg::ExprNode &second) const {}
  bool ExprProcessor::process_logical_and(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &first,
      const proto_msg::ExprNode &second) const {}
  bool ExprProcessor::process_logical_or(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &first,
      const proto_msg::ExprNode &second) const {}
  bool ExprProcessor::process_multiply(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &first,
      const proto_msg::ExprNode &second) const {}
  bool ExprProcessor::process_not_equals(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &first,
      const proto_msg::ExprNode &second) const {}
  bool ExprProcessor::process_same_term(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &first,
      const proto_msg::ExprNode &second) const {}
  bool ExprProcessor::process_str_after(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &first,
      const proto_msg::ExprNode &second) const {}
  bool ExprProcessor::process_str_before(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &first,
      const proto_msg::ExprNode &second) const {}
  bool ExprProcessor::process_str_contains(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &first,
      const proto_msg::ExprNode &second) const {}
  bool ExprProcessor::process_str_data_type(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &first,
      const proto_msg::ExprNode &second) const {}
  bool ExprProcessor::process_str_ends_with(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &first,
      const proto_msg::ExprNode &second) const {}
  bool ExprProcessor::process_str_lang(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &first,
      const proto_msg::ExprNode &second) const {}
  bool ExprProcessor::process_str_starts_with(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &first,
      const proto_msg::ExprNode &second) const {}
  bool ExprProcessor::process_substract(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &first,
      const proto_msg::ExprNode &second) const {}
  bool ExprProcessor::process_conditional(
      const ExprProcessor::row_t &row, const proto_msg::ExprNode &first,
      const proto_msg::ExprNode &second, const proto_msg::ExprNode &third)
      const {}
  bool ExprProcessor::process_bnode(
      const ExprProcessor::row_t &row,
      const proto_msg::FunctionNode &function_node) const {}
  bool ExprProcessor::process_call(const ExprProcessor::row_t &row,
                                   const proto_msg::FunctionNode &function_node)
      const {}
  bool ExprProcessor::process_coalesce(
      const ExprProcessor::row_t &row,
      const proto_msg::FunctionNode &function_node) const {}
  bool ExprProcessor::process_function(
      const ExprProcessor::row_t &row,
      const proto_msg::FunctionNode &function_node) const {}
  bool ExprProcessor::process_regex(
      const ExprProcessor::row_t &row,
      const proto_msg::FunctionNode &function_node) const {}
  bool ExprProcessor::process_str_concat(
      const ExprProcessor::row_t &row,
      const proto_msg::FunctionNode &function_node) const {}
  bool ExprProcessor::process_str_replace(
      const ExprProcessor::row_t &row,
      const proto_msg::FunctionNode &function_node) const {}
  bool ExprProcessor::process_str_substring(
      const ExprProcessor::row_t &row,
      const proto_msg::FunctionNode &function_node) const {}

  bool ExprProcessor::process_function_node(
      const ExprProcessor::row_t &row,
      const proto_msg::FunctionNode &function_node) const {
    switch (function_node.function_op()) {
    case proto_msg::FunctionOP::IS_LITERAL:
      return process_is_literal(row, function_node.exprs(0));
    case proto_msg::FunctionOP::BOUND:
      return process_bound(row, function_node.exprs(0));
    case proto_msg::FunctionOP::FUN_IRI:
      return process_fun_iri(row, function_node.exprs(0));
    case proto_msg::FunctionOP::IS_BLANK:
      return process_is_blank(row, function_node.exprs(0));
    case proto_msg::FunctionOP::IS_IRI:
      return process_is_iri(row, function_node.exprs(0));
    case proto_msg::FunctionOP::IS_NUMERIC:
      return process_is_numeric(row, function_node.exprs(0));
    case proto_msg::FunctionOP::LANG:
      return process_lang(row, function_node.exprs(0));
    case proto_msg::FunctionOP::LOGICAL_NOT:
      return process_logical_not(row, function_node.exprs(0));
    case proto_msg::FunctionOP::NUM_ABS:
      return process_num_abs(row, function_node.exprs(0));
    case proto_msg::FunctionOP::NUM_CEILING:
      return process_num_ceiling(row, function_node.exprs(0));
    case proto_msg::FunctionOP::NUM_FLOOR:
      return process_num_floor(row, function_node.exprs(0));
    case proto_msg::FunctionOP::NUM_ROUND:
      return process_num_round(row, function_node.exprs(0));
    case proto_msg::FunctionOP::STR:
      return process_str(row, function_node.exprs(0));
    case proto_msg::FunctionOP::STR_ENCODE_FOR_URI:
      return process_str_encode_for_uri(row, function_node.exprs(0));
    case proto_msg::FunctionOP::STR_LENGTH:
      return process_str_length(row, function_node.exprs(0));
    case proto_msg::FunctionOP::STR_LOWER_CASE:
      return process_str_lowercase(row, function_node.exprs(0));
    case proto_msg::FunctionOP::STR_UPPER_CASE:
      return process_str_uppercase(row, function_node.exprs(0));
    case proto_msg::FunctionOP::UNARY_MINUS:
      return process_unary_minus(row, function_node.exprs(0));
    case proto_msg::FunctionOP::UNARY_PLUS:
      return process_unary_plus(row, function_node.exprs(0));
    case proto_msg::FunctionOP::DIGEST_SHA1:
      return process_digest_sha1(row, function_node.exprs(0));
    case proto_msg::FunctionOP::DIGEST_MD5:
      return process_digest_md5(row, function_node.exprs(0));
    case proto_msg::FunctionOP::DIGEST_SHA224:
      return process_sha224(row, function_node.exprs(0));
    case proto_msg::FunctionOP::DIGEST_SHA256:
      return process_sha256(row, function_node.exprs(0));
    case proto_msg::FunctionOP::DIGEST_SHA384:
      return process_sha384(row, function_node.exprs(0));
    case proto_msg::FunctionOP::DIGEST_SHA512:
      return process_sha512(row, function_node.exprs(0));
    case proto_msg::FunctionOP::LANG_MATCHES:
      return process_lang_matches(row, function_node.exprs(0),
                                  function_node.exprs(1));
    case proto_msg::FunctionOP::ADD:
      return process_add(row, function_node.exprs(0), function_node.exprs(1));
    case proto_msg::FunctionOP::CAST:
      return process_cast(row, function_node.exprs(0), function_node.exprs(1));
    case proto_msg::FunctionOP::DIVIDE:
      return process_divide(row, function_node.exprs(0),
                            function_node.exprs(1));
    case proto_msg::FunctionOP::EQUALS:
      return process_equals(row, function_node.exprs(0),
                            function_node.exprs(1));
    case proto_msg::FunctionOP::GREATER_THAN:
      return process_greater_than(row, function_node.exprs(0),
                                  function_node.exprs(1));
    case proto_msg::FunctionOP::GREATER_THAN_OR_EQUAL:
      return process_greater_than_or_equal(row, function_node.exprs(0),
                                           function_node.exprs(1));
    case proto_msg::FunctionOP::LESS_THAN:
      return process_less_than(row, function_node.exprs(0),
                               function_node.exprs(1));
    case proto_msg::FunctionOP::LESS_THAN_OR_EQUAL:
      return process_less_than_or_equal(row, function_node.exprs(0),
                                        function_node.exprs(1));
    case proto_msg::FunctionOP::LOGICAL_AND:
      return process_logical_and(row, function_node.exprs(0),
                                 function_node.exprs(1));
    case proto_msg::FunctionOP::LOGICAL_OR:
      return process_logical_or(row, function_node.exprs(0),
                                function_node.exprs(1));
    case proto_msg::FunctionOP::MULTIPLY:
      return process_multiply(row, function_node.exprs(0),
                              function_node.exprs(1));
    case proto_msg::FunctionOP::NOT_EQUALS:
      return process_not_equals(row, function_node.exprs(0),
                                function_node.exprs(1));
    case proto_msg::FunctionOP::SAME_TERM:
      return process_same_term(row, function_node.exprs(0),
                               function_node.exprs(1));
    case proto_msg::FunctionOP::STR_AFTER:
      return process_str_after(row, function_node.exprs(0),
                               function_node.exprs(1));
    case proto_msg::FunctionOP::STR_BEFORE:
      return process_str_before(row, function_node.exprs(0),
                                function_node.exprs(1));
    case proto_msg::FunctionOP::STR_CONTAINS:
      return process_str_contains(row, function_node.exprs(0),
                                  function_node.exprs(1));
    case proto_msg::FunctionOP::STR_DATA_TYPE:
      return process_str_data_type(row, function_node.exprs(0),
                                   function_node.exprs(1));
    case proto_msg::FunctionOP::STR_ENDS_WITH:
      return process_str_ends_with(row, function_node.exprs(0),
                                   function_node.exprs(1));
    case proto_msg::FunctionOP::STR_LANG:
      return process_str_lang(row, function_node.exprs(0),
                              function_node.exprs(1));
    case proto_msg::FunctionOP::STR_STARTS_WITH:
      return process_str_starts_with(row, function_node.exprs(0),
                                     function_node.exprs(1));
    case proto_msg::FunctionOP::SUBSTRACT:
      return process_substract(row, function_node.exprs(0),
                               function_node.exprs(1));
    case proto_msg::FunctionOP::CONDITIONAL:
      return process_conditional(row, function_node.exprs(0),
                                 function_node.exprs(1),
                                 function_node.exprs(2));
    case proto_msg::FunctionOP::BNODE:
      return process_bnode(row, function_node);
    case proto_msg::FunctionOP::CALL:
      return process_call(row, function_node);
    case proto_msg::FunctionOP::COALESCE:
      return process_coalesce(row, function_node);
    case proto_msg::FunctionOP::FUNCTION:
      return process_function(row, function_node);
    case proto_msg::FunctionOP::REGEX:
      return process_regex(row, function_node);
    case proto_msg::FunctionOP::STR_CONCAT:
      return process_str_concat(row, function_node);
    case proto_msg::FunctionOP::STR_REPLACE:
      return process_str_replace(row, function_node);
    case proto_msg::FunctionOP::STR_SUBSTRING:
      return process_str_substring(row, function_node);
    default:
      throw std::runtime_error("Unknown function op : " +
                               std::to_string(function_node.function_op()));
    }
  }

  bool ExprProcessor::evaluate(const std::vector<unsigned long> &row) const {
    if (expr_node.expr_case() == proto_msg::ExprNode::kTermNode) {
      throw std::runtime_error(
          "ExprProcessor::execute: Base node can't be a term");
    }

    return process_function_node(row, expr_node.function_node());
  }