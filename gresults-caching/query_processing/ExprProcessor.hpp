
//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_EXPR_PROCESSOR_HPP
#define RDFCACHEK2_EXPR_PROCESSOR_HPP

#include <memory>
#include <unordered_map>
#include <string>

#include <request_msg.pb.h>

#include "ResultTable.hpp"
#include "VarIndexManager.hpp"
#include "PredicatesCacheManager.hpp"
#include "expr/BoolExprEval.hpp"
#include "expr/EvalData.hpp"

class ExprProcessor {
  EvalData eval_data;
  const proto_msg::ExprNode &expr_node;

  using row_t = std::vector<unsigned long>;

public:
  ExprProcessor(const ResultTable &table,
                const proto_msg::ExprNode &expr_node,
                const VarIndexManager &vim,
                const PredicatesCacheManager &cm,
                const std::unordered_map<std::string, unsigned long> &var_pos_mapping);

  bool evaluate(const std::vector<unsigned long> &row) const;
  std::unique_ptr<BoolExprEval> create_evaluator(const proto_msg::FunctionNode &function_node);

private:
bool process_function_node(const row_t &row, const proto_msg::FunctionNode &function_node) const;

bool process_is_literal(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_bound(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_data_type(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_date_time_day(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_date_time_hours(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_date_time_month(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_date_time_tz(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_date_time_year(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_fun_iri(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_is_blank(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_is_iri(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_is_numeric(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_lang(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_logical_not(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_num_abs(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_num_ceiling(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_num_floor(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_num_round(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_str(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_str_encode_for_uri(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_str_length(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_str_lowercase(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_str_uppercase(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_unary_minus(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_unary_plus(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_digest_sha1(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_digest_md5(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_sha224(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_sha256(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_sha384(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_sha512(const row_t &row, const proto_msg::ExprNode &node) const;
bool process_lang_matches(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_add(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_cast(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_divide(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_equals(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_greater_than(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_greater_than_or_equal(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_less_than(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_less_than_or_equal(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_logical_and(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_logical_or(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_multiply(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_not_equals(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_same_term(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_str_after(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_str_before(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_str_contains(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_str_data_type(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_str_ends_with(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_str_lang(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_str_starts_with(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_substract(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second) const;
bool process_conditional(const row_t &row, const proto_msg::ExprNode &first, const proto_msg::ExprNode &second,const proto_msg::ExprNode &third) const;
bool process_bnode(const row_t &row, const proto_msg::FunctionNode &function_node) const;
bool process_call(const row_t &row, const proto_msg::FunctionNode &function_node) const;
bool process_coalesce(const row_t &row, const proto_msg::FunctionNode &function_node) const;
bool process_function(const row_t &row, const proto_msg::FunctionNode &function_node) const;
bool process_regex(const row_t &row, const proto_msg::FunctionNode &function_node) const;
bool process_str_concat(const row_t &row, const proto_msg::FunctionNode &function_node) const;
bool process_str_replace(const row_t &row, const proto_msg::FunctionNode &function_node) const;
bool process_str_substring(const row_t &row, const proto_msg::FunctionNode &function_node) const;
};

#endif /* RDFCACHEK2_EXPR_PROCESSOR_HPP */