//
// Created by cristobal on 4/21/21.
//

#ifndef RDFCACHEK2_TERMRESOURCE_HPP
#define RDFCACHEK2_TERMRESOURCE_HPP

#include "query_processing/ExprDataType.hpp"
#include <RDFTriple.hpp>
#include <query_processing/DateInfo.hpp>

class BooleanResource;
class ConcreteRDFResource;
class DataTypeResource;
class DateTimeResource;
class DayTimeDurationResource;
class DoubleResource;
class FloatResource;
class IntegerResource;
class IRIResource;
class NullResource;
class StringLiteralLangResource;
class StringLiteralResource;

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
  virtual bool is_datetime() const;
  virtual int diff_compare(const TermResource &rhs) const;

  virtual const std::string &get_literal_string() const;
  virtual const std::string &get_literal_lang_string() const;
  virtual const std::string &get_iri_string() const;
  virtual const std::string &get_lang_tag() const;
  virtual std::string get_content_string_copy() const;
  virtual const RDFResource &get_resource() const;
  virtual RDFResource get_resource_clone() const;
  virtual ExprDataType get_datatype() const;
  virtual int get_integer() const;
  virtual float get_float() const;
  virtual double get_double() const;
  virtual bool get_boolean() const;
  virtual DateInfo get_dateinfo() const;

  static std::shared_ptr<TermResource> null();

  virtual bool contains(TermResource &pattern_resource) const;
  virtual int reverse_diff_compare(const BooleanResource &) const;
  virtual int reverse_diff_compare(const ConcreteRDFResource &) const;
  virtual int reverse_diff_compare(const DataTypeResource &) const;
  virtual int reverse_diff_compare(const DateTimeResource &) const;
  virtual int reverse_diff_compare(const DayTimeDurationResource &) const;
  virtual int reverse_diff_compare(const DoubleResource &) const;
  virtual int reverse_diff_compare(const IntegerResource &) const;
  virtual int reverse_diff_compare(const FloatResource &) const;
  virtual int reverse_diff_compare(const IRIResource &) const;
  virtual int reverse_diff_compare(const StringLiteralLangResource &) const;
  virtual int reverse_diff_compare(const StringLiteralResource &) const;

  virtual std::shared_ptr<TermResource> cast_to(ExprDataType expr_data_type);
};

#endif // RDFCACHEK2_TERMRESOURCE_HPP
