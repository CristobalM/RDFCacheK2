//
// Created by cristobal on 4/21/21.
//

#include "DataTypeResource.hpp"
#include "StringLiteralResource.hpp"

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
int DataTypeResource::reverse_diff_compare(
    const DataTypeResource &data_type_resource) const {
  return data_type_resource == *this ? 0 : -1;
}
std::shared_ptr<TermResource>
DataTypeResource::cast_to(ExprDataType expr_data_type) {

  if (expr_data_type != EDT_STRING)
    return TermResource::null();

  switch (datatype) {

  case EDT_INTEGER:
    return std::make_shared<StringLiteralResource>("xsd:integer");
  case EDT_DECIMAL:
    return std::make_shared<StringLiteralResource>("xsd:decimal");
  case EDT_FLOAT:
    return std::make_shared<StringLiteralResource>("xsd:float");
  case EDT_DOUBLE:
    return std::make_shared<StringLiteralResource>("xsd:double");
  case EDT_STRING:
    return std::make_shared<StringLiteralResource>("xsd:string");
  case EDT_BOOLEAN:
    return std::make_shared<StringLiteralResource>("xsd:boolean");
  case EDT_DATETIME:
    return std::make_shared<StringLiteralResource>("xsd:dateTime");
  case EDT_UNKNOWN:
  default:
    return TermResource::null();
  }
}
int DataTypeResource::diff_compare(const TermResource &rhs) const {
  return rhs.reverse_diff_compare(*this);
}
