
//
// Created by Cristobal Miranda, 2021
//

#ifndef RDFCACHEK2_EXPR_EVAL_HPP
#define RDFCACHEK2_EXPR_EVAL_HPP

#include <RDFTriple.hpp>

#include <memory>
#include <vector>

#include <unicode/smpdtfmt.h>

#include "../ExprProcessorPersistentData.hpp"
#include "EvalData.hpp"
#include "TermResource.hpp"

class ExprEval {
protected:
  std::vector<std::unique_ptr<ExprEval>> children;

  const EvalData &eval_data;
  const proto_msg::ExprNode &expr_node;

  bool with_error;

public:
  using row_t = std::vector<unsigned long>;
  virtual ~ExprEval() = default;

  ExprEval(const EvalData &eval_data, const proto_msg::ExprNode &expr_node);

  virtual std::unique_ptr<TermResource> eval_resource(const row_t &row);
  virtual std::unique_ptr<TermResource> eval_datatype(const row_t &row);
  virtual bool eval_boolean(const row_t &row);
  virtual int eval_integer(const row_t &row);
  virtual float eval_float(const row_t &row);
  virtual double eval_double(const row_t &row);
  virtual DateInfo eval_date_time(const row_t &row);

  virtual void validate();
  virtual void init();

  void assert_fsize(int size);
  static void assert_is_rdf_term(const proto_msg::ExprNode &expr_node);
  static void assert_is_variable(const proto_msg::ExprNode &expr_node);
  static void assert_is_function(const proto_msg::ExprNode &expr_node);

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

  bool has_error() const;

  bool children_with_error() const;

  std::unique_ptr<TermResource> resource_with_error();
  bool bool_with_error();
  int integer_with_error();
  float float_with_error();
  double double_with_error();
};

#endif