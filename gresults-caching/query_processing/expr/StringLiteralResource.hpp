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
  bool is_string_literal() const override;
  const std::string &get_literal_string() const override;
  ExprDataType get_datatype() const override;
  bool operator==(const TermResource &rhs) const override;
  bool contains(TermResource &pattern_resource) const override;
};

#endif // RDFCACHEK2_STRINGLITERALRESOURCE_HPP
