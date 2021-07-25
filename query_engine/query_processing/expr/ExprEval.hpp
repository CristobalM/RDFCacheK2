
//
// Created by Cristobal Miranda, 2021
//

#ifndef RDF_CACHE_K2_EXPR_EVAL_HPP
#define RDF_CACHE_K2_EXPR_EVAL_HPP

#include <RDFTriple.hpp>

#include <memory>
#include <vector>

#include <unicode/smpdtfmt.h>

#include "../ParsingUtils.hpp"
#include "EvalData.hpp"
#include "query_processing/resources/TermResource.hpp"

class ExprEval {

protected:
  std::vector<std::unique_ptr<ExprEval>> children;

  const EvalData &eval_data;
  const proto_msg::ExprNode expr_node;

  bool with_error;

  bool with_constant_subtree;

  std::shared_ptr<TermResource> cached_resource;
  std::shared_ptr<TermResource> cached_datatype;
  bool cached_boolean;
  int cached_integer;
  float cached_float;
  double cached_double;
  DateInfo cached_date_info;

  bool was_resource_cached;
  bool was_datatype_cached;
  bool was_boolean_cached;
  bool was_integer_cached;
  bool was_float_cached;
  bool was_double_cached;
  bool was_date_info_cached;

public:
  using row_t = std::vector<unsigned long>;
  virtual ~ExprEval() = default;
  virtual void init();
  bool has_error() const;
  std::shared_ptr<TermResource> produce_resource(const row_t &row);
  std::shared_ptr<TermResource> produce_datatype(const row_t &row);
  bool produce_boolean(const row_t &row);
  int produce_integer(const row_t &row);
  float produce_float(const row_t &row);
  double produce_double(const row_t &row);
  DateInfo produce_date_time(const row_t &row);

  ExprEval(const EvalData &eval_data, proto_msg::ExprNode expr_node);
  virtual std::shared_ptr<TermResource> eval_resource(const row_t &row);
  virtual std::shared_ptr<TermResource> eval_datatype(const row_t &row);
  virtual bool eval_boolean(const row_t &row);
  virtual int eval_integer(const row_t &row);
  virtual float eval_float(const row_t &row);
  virtual double eval_double(const row_t &row);
  virtual DateInfo eval_date_time(const row_t &row);

  virtual void validate();

  static void assert_is_rdf_term(const proto_msg::ExprNode &expr_node);
  static void assert_is_variable(const proto_msg::ExprNode &expr_node);
  static void assert_is_function(const proto_msg::ExprNode &expr_node);
  static void assert_is_pattern_node(const proto_msg::ExprNode &node);

  template <typename T>
  static std::unique_ptr<ExprEval>
  create_eval_node_specific(const EvalData &eval_data,
                            const proto_msg::ExprNode &expr_node) {
    return std::make_unique<T>(eval_data, expr_node);
  }

  static std::unique_ptr<ExprEval>
  create_eval_node(const EvalData &eval_data,
                   const proto_msg::ExprNode &expr_node);

  void add_child(const proto_msg::ExprNode &child_node);
  void add_children(int count);
  void add_children();

  bool children_with_error() const;

  std::shared_ptr<TermResource> resource_with_error();
  bool bool_with_error();
  int integer_with_error();
  float float_with_error();
  double double_with_error();

protected:
  std::shared_ptr<TermResource> generate_from_eval_boolean(const row_t &row);
  std::shared_ptr<TermResource> generate_from_eval_integer(const row_t &row);
  std::shared_ptr<TermResource> generate_from_eval_double(const row_t &row);
  void assert_fun_size(int size);
  void assert_fun_size_gt(int size);
  void assert_fun_size_between_inclusive(int first, int second);

private:
  virtual bool has_constant_subtree();
};

#endif