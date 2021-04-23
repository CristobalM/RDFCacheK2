//
// Created by cristobal on 4/21/21.
//

#ifndef RDFCACHEK2_NULLRESOURCE_HPP
#define RDFCACHEK2_NULLRESOURCE_HPP

#include "TermResource.hpp"
class NullResource : public TermResource {
public:
  bool operator==(const TermResource &rhs) const override;
  bool is_null() const override;
};

#endif // RDFCACHEK2_NULLRESOURCE_HPP
