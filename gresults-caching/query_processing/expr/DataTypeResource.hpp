//
// Created by cristobal on 4/21/21.
//

#ifndef RDFCACHEK2_DATATYPERESOURCE_HPP
#define RDFCACHEK2_DATATYPERESOURCE_HPP

#include "TermResource.hpp"
#include "../ExprDataType.hpp"
class DataTypeResource : public TermResource {
  ExprDataType datatype;
public:

  DataTypeResource(ExprDataType datatype);

  bool operator==(const TermResource &rhs) const override;
  bool is_concrete() const override;
  const RDFResource &get_resource() const override;
  bool is_datatype() const override;
  ExprDataType get_datatype() const override;
};

#endif // RDFCACHEK2_DATATYPERESOURCE_HPP
