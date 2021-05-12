//
// Created by cristobal on 4/21/21.
//

#include "DataTypeResource.hpp"
bool DataTypeResource::operator==(const TermResource &rhs) const {
  return is_datatype() && datatype == rhs.get_datatype();
}
bool DataTypeResource::is_concrete() const { return false; }
DataTypeResource::DataTypeResource(ExprDataType datatype)
    : datatype(datatype) {}
bool DataTypeResource::is_datatype() const { return true; }
ExprDataType DataTypeResource::get_datatype() const { return datatype; }
std::shared_ptr<TermResource> DataTypeResource::create(ExprDataType datatype) {
  return std::make_shared<DataTypeResource>(datatype);
}

const std::string DataTypeResource::short_prefix = "xsd:";
const std::string DataTypeResource::long_prefix =
    "<http://www.w3.org/2001/XMLSchema#";
bool DataTypeResource::is_literal() const { return true; }
