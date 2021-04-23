//
// Created by cristobal on 4/21/21.
//

#ifndef RDFCACHEK2_TERMRESOURCE_HPP
#define RDFCACHEK2_TERMRESOURCE_HPP

#include <RDFTriple.hpp>
#include "../ExprDataType.hpp"
class TermResource {
public:
  virtual bool operator==(const TermResource &rhs) const = 0;
  virtual bool is_concrete() const;
  virtual bool is_datatype() const;
  virtual bool is_null() const;
  virtual const RDFResource &get_resource() const;
  virtual ExprDataType get_datatype() const;

  static std::unique_ptr<TermResource> null();
};

#endif // RDFCACHEK2_TERMRESOURCE_HPP
