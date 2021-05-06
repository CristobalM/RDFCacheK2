//
// Created by cristobal on 4/21/21.
//

#ifndef RDFCACHEK2_DATATYPERESOURCE_HPP
#define RDFCACHEK2_DATATYPERESOURCE_HPP

#include "../ExprDataType.hpp"
#include "TermResource.hpp"
class DataTypeResource : public TermResource {
  ExprDataType datatype;

public:
  static const std::string short_prefix;
  static const std::string long_prefix;

  DataTypeResource(ExprDataType datatype);
  bool operator==(const TermResource &rhs) const override;
  bool is_concrete() const override;
  bool is_datatype() const override;
  ExprDataType get_datatype() const override;

  static std::unique_ptr<TermResource> create(ExprDataType datatype);
  bool is_literal() const override;
};

#endif // RDFCACHEK2_DATATYPERESOURCE_HPP
