//
// Created by cristobal on 6/4/21.
//

#ifndef RDFCACHEK2_DATETIMERESOURCE_HPP
#define RDFCACHEK2_DATETIMERESOURCE_HPP

#include "TermResource.hpp"
#include <query_processing/DateInfo.hpp>
class DateTimeResource : public TermResource {
  DateInfo date_info;

public:
  explicit DateTimeResource(DateInfo date_info);
  bool is_datetime() const override;
  const std::string &get_literal_string() const override;
  RDFResource get_resource_clone() const override;
  ExprDataType get_datatype() const override;
  DateInfo get_dateinfo() const override;
  bool operator==(const TermResource &rhs) const override;
  int reverse_diff_compare(
      const DateTimeResource &date_time_resource) const override;
  std::shared_ptr<TermResource> cast_to(ExprDataType expr_data_type) override;
  int diff_compare(const TermResource &rhs) const override;
  bool can_cast_to_literal_string() const override;
  std::string get_content_string_copy() const override;
};

#endif // RDFCACHEK2_DATETIMERESOURCE_HPP
