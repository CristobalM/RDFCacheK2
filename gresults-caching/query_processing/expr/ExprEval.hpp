
//
// Created by Cristobal Miranda, 2021
//

#ifndef RDFCACHEK2_EXPR_EVAL_HPP
#define RDFCACHEK2_EXPR_EVAL_HPP

#include <RDFTriple.hpp>

#include <memory>
#include <vector>

#include "../ExprProcessorPersistentData.hpp"
#include "EvalData.hpp"

class ExprEval {
protected:
  std::vector<std::unique_ptr<ExprEval>> children;

  const EvalData &eval_data;
  const ExprProcessorPersistentData &persistent_data;
  const proto_msg::ExprNode &expr_node;

  bool with_error;

public:
  using row_t = std::vector<unsigned long>;
  virtual ~ExprEval() = default;

  ExprEval(const EvalData &eval_data,
           const ExprProcessorPersistentData &persistent_data,
           const proto_msg::ExprNode &expr_node);

  [[nodiscard]] virtual RDFResource eval_resource(const row_t &row) = 0;
  [[nodiscard]] virtual bool eval_boolean(const row_t &row) = 0;
  [[nodiscard]] virtual int eval_integer(const row_t &row) = 0;
  [[nodiscard]] virtual float eval_float(const row_t &row) = 0;
  [[nodiscard]] virtual double eval_double(const row_t &row) = 0;

  virtual void validate() {}
  virtual void init() { validate(); }

  void assert_fsize(int size);
  static void assert_is_rdf_term(const proto_msg::ExprNode &expr_node);
  static void assert_is_variable(const proto_msg::ExprNode &expr_node);
  static void assert_is_function(const proto_msg::ExprNode &expr_node);

  template <typename T>
  static std::unique_ptr<ExprEval>
  create_eval_node_specific(const EvalData &eval_data,
                            const ExprProcessorPersistentData &persistent_data,
                            const proto_msg::ExprNode &expr_node) {
    return std::make_unique<T>(eval_data, persistent_data, expr_node);
  }

  static std::unique_ptr<ExprEval>
  create_eval_node(const EvalData &eval_data,
                   const ExprProcessorPersistentData &persistent_data,
                   const proto_msg::ExprNode &expr_node);

  void add_child(const proto_msg::ExprNode &child_node);

  bool has_error() const;
};

#endif