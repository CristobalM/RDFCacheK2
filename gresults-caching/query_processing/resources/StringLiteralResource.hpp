//
// Created by cristobal on 4/25/21.
//

#ifndef RDFCACHEK2_STRINGLITERALRESOURCE_HPP
#define RDFCACHEK2_STRINGLITERALRESOURCE_HPP

#include "TermResource.hpp"
class StringLiteralResource : public TermResource {
  std::string value;
  ExprDataType data_type;

public:
  StringLiteralResource(std::string &&value, ExprDataType data_type);
  explicit StringLiteralResource(std::string &&value);
  bool is_string_literal() const override;
  const std::string &get_literal_string() const override;
  ExprDataType get_datatype() const override;
  bool operator==(const TermResource &rhs) const override;
  bool contains(TermResource &pattern_resource) const override;
  bool is_literal() const override;
  RDFResource get_resource_clone() const override;
  std::shared_ptr<TermResource> cast_to(ExprDataType expr_data_type) override;
  bool is_number();
};

#endif // RDFCACHEK2_STRINGLITERALRESOURCE_HPP
