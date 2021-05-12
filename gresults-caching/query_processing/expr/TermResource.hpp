//
// Created by cristobal on 4/21/21.
//

#ifndef RDFCACHEK2_TERMRESOURCE_HPP
#define RDFCACHEK2_TERMRESOURCE_HPP

#include "../ExprDataType.hpp"
#include <RDFTriple.hpp>

class TermResource {
public:
  virtual ~TermResource() = default;
  virtual bool operator==(const TermResource &rhs) const = 0;
  bool operator!=(const TermResource &rhs) const;
  virtual bool is_concrete() const;
  virtual bool is_datatype() const;
  virtual bool is_null() const;
  virtual bool is_day_time_duration() const;
  virtual bool is_string_literal() const;
  virtual bool is_string_literal_lang() const;
  virtual bool is_integer() const;
  virtual bool is_float() const;
  virtual bool is_double() const;
  virtual bool is_numeric() const;
  virtual bool is_boolean() const;
  virtual bool is_iri() const;
  virtual bool is_literal() const;

  virtual const std::string &get_literal_string() const;
  virtual const std::string &get_literal_lang_string() const;
  virtual const std::string &get_iri_string() const;
  virtual const std::string &get_lang_tag() const;
  virtual const RDFResource &get_resource() const;
  virtual RDFResource get_resource_clone() const;
  virtual ExprDataType get_datatype() const;
  virtual int get_integer() const;
  virtual float get_float() const;
  virtual double get_double() const;
  virtual bool get_boolean() const;

  static std::shared_ptr<TermResource> null();

  virtual bool contains(TermResource &pattern_resource) const;
};

#endif // RDFCACHEK2_TERMRESOURCE_HPP
