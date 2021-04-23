//
// Created by cristobal on 4/21/21.
//

#include "DataTypeResource.hpp"
bool DataTypeResource::operator==(const TermResource &rhs) const {
  return is_datatype() && datatype == rhs.get_datatype();
}
bool DataTypeResource::is_concrete() const { return false; }
DataTypeResource::DataTypeResource(ExprDataType datatype) : datatype(datatype) {}
bool DataTypeResource::is_datatype() const {
  return true;
}
ExprDataType DataTypeResource::get_datatype() const {
  return datatype;
}
