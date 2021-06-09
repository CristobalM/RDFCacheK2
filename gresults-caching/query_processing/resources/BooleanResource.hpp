//
// Created by cristobal on 4/26/21.
//

#ifndef RDFCACHEK2_BOOLEANRESOURCE_HPP
#define RDFCACHEK2_BOOLEANRESOURCE_HPP

#include "TermResource.hpp"
class BooleanResource : public TermResource {
  bool value;

public:
  explicit BooleanResource(bool value);

  bool operator==(const TermResource &rhs) const override;
  bool is_boolean() const override;
  bool get_boolean() const override;
  RDFResource get_resource_clone() const override;
  int diff_compare(const TermResource &rhs) const override;
  int reverse_diff_compare(
      const BooleanResource &boolean_resource) const override;
  std::shared_ptr<TermResource> cast_to(ExprDataType expr_data_type) override;
};

#endif // RDFCACHEK2_BOOLEANRESOURCE_HPP
