
//
// Created by Cristobal Miranda, 2021
//

#include "ExprEval.hpp"
#include <query_processing/expr/LangMatchesEval.hpp>
#include <query_processing/expr/LogicalNotEval.hpp>

#include "AddEval.hpp"
#include "BNodeEval.hpp"
#include "BoundEval.hpp"
#include "CallEval.hpp"
#include "CastEval.hpp"
#include "CoalesceEval.hpp"
#include "ConditionalEval.hpp"
#include "DataTypeEval.hpp"
#include "DateTimeDayEval.hpp"
#include "DateTimeHoursEval.hpp"
#include "DateTimeMinutesEval.hpp"
#include "DateTimeMonthEval.hpp"
#include "DateTimeTZEval.hpp"
#include "DateTimeYearEval.hpp"
#include "DigestMD5Eval.hpp"
#include "DigestSHA1Eval.hpp"
#include "DigestSHA224Eval.hpp"
#include "DigestSHA256Eval.hpp"
#include "DigestSHA384Eval.hpp"
#include "DigestSHA512Eval.hpp"
#include "DivideEval.hpp"
#include "EqualsEval.hpp"
#include "FunIRIEval.hpp"
#include "FunctionEval.hpp"
#include "GreaterThanEval.hpp"
#include "GreaterThanOrEqualEval.hpp"
#include "IsBlankEval.hpp"
#include "IsIRIEval.hpp"
#include "IsLiteralEval.hpp"
#include "IsNumericEval.hpp"
#include "LangEval.hpp"
#include "LessThanEval.hpp"
#include "LessThanOrEqualEval.hpp"
#include "LogicalAndEval.hpp"
#include "LogicalOrEval.hpp"
#include "MultiplyEval.hpp"
#include "NotEqualsEval.hpp"
#include "NumAbsEval.hpp"
#include "NumCeilingEval.hpp"
#include "NumFloorEval.hpp"
#include "NumRoundEval.hpp"
#include "OneOfBaseEval.hpp"
#include "RegexEval.hpp"
#include "SameTermEval.hpp"
#include "SecuredFunctionEval.hpp"
#include "StrAfterEval.hpp"
#include "StrBeforeEval.hpp"
#include "StrConcatEval.hpp"
#include "StrContainsEval.hpp"
#include "StrDataTypeEval.hpp"
#include "StrEncodeForURIEval.hpp"
#include "StrEndsWithEval.hpp"
#include "StrEval.hpp"
#include "StrLangEval.hpp"
#include "StrLengthEval.hpp"
#include "StrLowerCaseEval.hpp"
#include "StrReplaceEval.hpp"
#include "StrStartsWithEval.hpp"
#include "StrSubstringEval.hpp"
#include "StrUpperCaseCaseEval.hpp"
#include "SubtractEval.hpp"
#include "TermEval.hpp"
#include "UnaryMinusEval.hpp"
#include "UnaryPlusEval.hpp"

ExprEval::ExprEval(const EvalData &eval_data,
                   ExprProcessorPersistentData &persistent_data,
                   const proto_msg::ExprNode &expr_node)
    : eval_data(eval_data), persistent_data(persistent_data),
      expr_node(expr_node), with_error(false) {}
void ExprEval::assert_fsize(int size) {
  if (expr_node.function_node().exprs_size() != size)
    throw std::runtime_error(
        "Invalid size " +
        std::to_string(expr_node.function_node().exprs_size()) + ", expected " +
        std::to_string(size));
}
void ExprEval::assert_is_rdf_term(const proto_msg::ExprNode &expr_node) {
  if (expr_node.expr_case() != proto_msg::ExprNode::kTermNode)
    throw std::runtime_error(
        "Assertion failed, expected a rdf term, got a function");
}
void ExprEval::assert_is_function(const proto_msg::ExprNode &expr_node) {
  if (expr_node.expr_case() != proto_msg::ExprNode::kFunctionNode)
    throw std::runtime_error(
        "Assertion failed, expected a function, got a rdf term");
}
void ExprEval::assert_is_variable(const proto_msg::ExprNode &expr_node) {
  assert_is_rdf_term(expr_node);
  if (expr_node.term_node().term_type() != proto_msg::TermType::VARIABLE)
    throw std::runtime_error("Assertion failed, expected a variable");
}

std::unique_ptr<ExprEval>
ExprEval::create_eval_node(const EvalData &eval_data,
                           const ExprProcessorPersistentData &persistent_data,
                           const proto_msg::ExprNode &child_node) {
  if (child_node.expr_case() == proto_msg::ExprNode::kTermNode) {
    return create_eval_node_specific<TermEval>(eval_data, persistent_data,
                                               child_node);
  }
  const auto &function_node = child_node.function_node();
  switch (function_node.function_op()) {

  case proto_msg::IS_LITERAL:
    return create_eval_node_specific<IsLiteralEval>(eval_data, persistent_data,
                                                    child_node);
  case proto_msg::BOUND:
    return create_eval_node_specific<BoundEval>(eval_data, persistent_data,
                                                child_node);
  case proto_msg::DATA_TYPE:
    return create_eval_node_specific<DataTypeEval>(eval_data, persistent_data,
                                                   child_node);
  case proto_msg::DATE_TIME_DAY:
    return create_eval_node_specific<DateTimeDayEval>(
        eval_data, persistent_data, child_node);
  case proto_msg::DATE_TIME_HOURS:
    return create_eval_node_specific<DateTimeHoursEval>(
        eval_data, persistent_data, child_node);
  case proto_msg::DATE_TIME_MINUTES:
    return create_eval_node_specific<DateTimeMinutesEval>(
        eval_data, persistent_data, child_node);
  case proto_msg::DATE_TIME_MONTH:
    return create_eval_node_specific<DateTimeMonthEval>(
        eval_data, persistent_data, child_node);
  case proto_msg::DATE_TIME_TZ:
    return create_eval_node_specific<DateTimeTZEval>(eval_data, persistent_data,
                                                     child_node);
  case proto_msg::DATE_TIME_YEAR:
    return create_eval_node_specific<DateTimeYearEval>(
        eval_data, persistent_data, child_node);
  case proto_msg::FUN_IRI:
    return create_eval_node_specific<FunIRIEval>(eval_data, persistent_data,
                                                 child_node);
  case proto_msg::IS_BLANK:
    return create_eval_node_specific<IsBlankEval>(eval_data, persistent_data,
                                                  child_node);
  case proto_msg::IS_IRI:
    return create_eval_node_specific<IsIRIEval>(eval_data, persistent_data,
                                                child_node);
  case proto_msg::IS_NUMERIC:
    return create_eval_node_specific<IsNumericEval>(eval_data, persistent_data,
                                                    child_node);
  case proto_msg::LANG:
    return create_eval_node_specific<LangEval>(eval_data, persistent_data,
                                               child_node);
  case proto_msg::LOGICAL_NOT:
    return create_eval_node_specific<LogicalNotEval>(eval_data, persistent_data,
                                                     child_node);
  case proto_msg::NUM_ABS:
    return create_eval_node_specific<NumAbsEval>(eval_data, persistent_data,
                                                 child_node);
  case proto_msg::NUM_CEILING:
    return create_eval_node_specific<NumCeilingEval>(eval_data, persistent_data,
                                                     child_node);
  case proto_msg::NUM_FLOOR:
    return create_eval_node_specific<NumFloorEval>(eval_data, persistent_data,
                                                   child_node);
  case proto_msg::NUM_ROUND:
    return create_eval_node_specific<NumRoundEval>(eval_data, persistent_data,
                                                   child_node);
  case proto_msg::STR:
    return create_eval_node_specific<StrEval>(eval_data, persistent_data,
                                              child_node);
  case proto_msg::STR_ENCODE_FOR_URI:
    return create_eval_node_specific<StrEncodeForURIEval>(
        eval_data, persistent_data, child_node);
  case proto_msg::STR_LENGTH:
    return create_eval_node_specific<StrLengthEval>(eval_data, persistent_data,
                                                    child_node);
  case proto_msg::STR_LOWER_CASE:
    return create_eval_node_specific<StrLowerCaseEval>(
        eval_data, persistent_data, child_node);
  case proto_msg::STR_UPPER_CASE:
    return create_eval_node_specific<StrUpperCaseCaseEval>(
        eval_data, persistent_data, child_node);
  case proto_msg::UNARY_MINUS:
    return create_eval_node_specific<UnaryMinusEval>(eval_data, persistent_data,
                                                     child_node);
  case proto_msg::UNARY_PLUS:
    return create_eval_node_specific<UnaryPlusEval>(eval_data, persistent_data,
                                                    child_node);
  case proto_msg::DIGEST_SHA1:
    return create_eval_node_specific<DigestSHA1Eval>(eval_data, persistent_data,
                                                     child_node);
  case proto_msg::DIGEST_MD5:
    return create_eval_node_specific<DigestMD5Eval>(eval_data, persistent_data,
                                                    child_node);
  case proto_msg::DIGEST_SHA224:
    return create_eval_node_specific<DigestSHA224Eval>(
        eval_data, persistent_data, child_node);
  case proto_msg::DIGEST_SHA256:
    return create_eval_node_specific<DigestSHA256Eval>(
        eval_data, persistent_data, child_node);
  case proto_msg::DIGEST_SHA384:
    return create_eval_node_specific<DigestSHA384Eval>(
        eval_data, persistent_data, child_node);
  case proto_msg::DIGEST_SHA512:
    return create_eval_node_specific<DigestSHA512Eval>(
        eval_data, persistent_data, child_node);
  case proto_msg::LANG_MATCHES:
    return create_eval_node_specific<LangMatchesEval>(
        eval_data, persistent_data, child_node);
  case proto_msg::ADD:
    return create_eval_node_specific<AddEval>(eval_data, persistent_data,
                                              child_node);
  case proto_msg::CAST:
    return create_eval_node_specific<CastEval>(eval_data, persistent_data,
                                               child_node);
  case proto_msg::DIVIDE:
    return create_eval_node_specific<DivideEval>(eval_data, persistent_data,
                                                 child_node);
  case proto_msg::EQUALS:
    return create_eval_node_specific<EqualsEval>(eval_data, persistent_data,
                                                 child_node);
  case proto_msg::GREATER_THAN:
    return create_eval_node_specific<GreaterThanEval>(
        eval_data, persistent_data, child_node);
  case proto_msg::GREATER_THAN_OR_EQUAL:
    return create_eval_node_specific<GreaterThanOrEqualEval>(
        eval_data, persistent_data, child_node);
  case proto_msg::LESS_THAN:
    return create_eval_node_specific<LessThanEval>(eval_data, persistent_data,
                                                   child_node);
  case proto_msg::LESS_THAN_OR_EQUAL:
    return create_eval_node_specific<LessThanOrEqualEval>(
        eval_data, persistent_data, child_node);
  case proto_msg::LOGICAL_AND:
    return create_eval_node_specific<LogicalAndEval>(eval_data, persistent_data,
                                                     child_node);
  case proto_msg::LOGICAL_OR:
    return create_eval_node_specific<LogicalOrEval>(eval_data, persistent_data,
                                                    child_node);
  case proto_msg::MULTIPLY:
    return create_eval_node_specific<MultiplyEval>(eval_data, persistent_data,
                                                   child_node);
  case proto_msg::NOT_EQUALS:
    return create_eval_node_specific<NotEqualsEval>(eval_data, persistent_data,
                                                    child_node);
  case proto_msg::SAME_TERM:
    return create_eval_node_specific<SameTermEval>(eval_data, persistent_data,
                                                   child_node);
  case proto_msg::STR_AFTER:
    return create_eval_node_specific<StrAfterEval>(eval_data, persistent_data,
                                                   child_node);
  case proto_msg::STR_BEFORE:
    return create_eval_node_specific<StrBeforeEval>(eval_data, persistent_data,
                                                    child_node);
  case proto_msg::STR_CONTAINS:
    return create_eval_node_specific<StrContainsEval>(
        eval_data, persistent_data, child_node);
  case proto_msg::STR_DATA_TYPE:
    return create_eval_node_specific<StrDataTypeEval>(
        eval_data, persistent_data, child_node);
  case proto_msg::STR_ENDS_WITH:
    return create_eval_node_specific<StrEndsWithEval>(
        eval_data, persistent_data, child_node);
  case proto_msg::STR_LANG:
    return create_eval_node_specific<StrLangEval>(eval_data, persistent_data,
                                                  child_node);
  case proto_msg::STR_STARTS_WITH:
    return create_eval_node_specific<StrStartsWithEval>(
        eval_data, persistent_data, child_node);
  case proto_msg::SUBSTRACT:
    return create_eval_node_specific<SubtractEval>(eval_data, persistent_data,
                                                   child_node);
  case proto_msg::CONDITIONAL:
    return create_eval_node_specific<ConditionalEval>(
        eval_data, persistent_data, child_node);
  case proto_msg::SECURED_FUNCTION:
    return create_eval_node_specific<SecuredFunctionEval>(
        eval_data, persistent_data, child_node);
  case proto_msg::BNODE:
    return create_eval_node_specific<BNodeEval>(eval_data, persistent_data,
                                                child_node);
  case proto_msg::CALL:
    return create_eval_node_specific<CallEval>(eval_data, persistent_data,
                                               child_node);
  case proto_msg::COALESCE:
    return create_eval_node_specific<CoalesceEval>(eval_data, persistent_data,
                                                   child_node);
  case proto_msg::FUNCTION:
    return create_eval_node_specific<FunctionEval>(eval_data, persistent_data,
                                                   child_node);
  case proto_msg::ONE_OF_BASE:
    return create_eval_node_specific<OneOfBaseEval>(eval_data, persistent_data,
                                                    child_node);
  case proto_msg::REGEX:
    return create_eval_node_specific<RegexEval>(eval_data, persistent_data,
                                                child_node);
  case proto_msg::STR_CONCAT:
    return create_eval_node_specific<StrConcatEval>(eval_data, persistent_data,
                                                    child_node);
  case proto_msg::STR_REPLACE:
    return create_eval_node_specific<StrReplaceEval>(eval_data, persistent_data,
                                                     child_node);
  case proto_msg::STR_SUBSTRING:
    return create_eval_node_specific<StrSubstringEval>(
        eval_data, persistent_data, child_node);
  default:
    throw std::runtime_error("Unknown function op");
  }
}

void ExprEval::add_child(const proto_msg::ExprNode &child_node) {
  children.push_back(create_eval_node(eval_data, persistent_data, child_node));
}

bool ExprEval::has_error() const { return with_error; }
void ExprEval::add_children(int count) {
  for (int i = 0; i < count; i++) {
    add_child(expr_node.function_node().exprs(i));
  }
}
void ExprEval::add_children() {
  add_children(expr_node.function_node().exprs_size());
}
void ExprEval::init() { validate(); }
void ExprEval::validate() {}
std::unique_ptr<TermResource> ExprEval::eval_resource(const ExprEval::row_t &) {
  throw std::runtime_error("ExprEval::eval_resource not implemented");
}
bool ExprEval::eval_boolean(const ExprEval::row_t &) {
  throw std::runtime_error("ExprEval::eval_boolean not implemented");
}
int ExprEval::eval_integer(const ExprEval::row_t &) {
  throw std::runtime_error("ExprEval::eval_integer not implemented");
}
float ExprEval::eval_float(const ExprEval::row_t &) {
  throw std::runtime_error("ExprEval::eval_float not implemented");
}
double ExprEval::eval_double(const ExprEval::row_t &) {
  throw std::runtime_error("ExprEval::eval_double not implemented");
}
UDate ExprEval::eval_date_time(const ExprEval::row_t &) {
  throw std::runtime_error("ExprEval::eval_date_time not implemented");
}
std::unique_ptr<TermResource> ExprEval::eval_datatype(const ExprEval::row_t &) {
  throw std::runtime_error("ExprEval::eval_datatype not implemented");
}

bool ExprEval::children_with_error() const {
  for (const auto &child : children) {
    if (child->has_error())
      return true;
  }
  return false;
}
