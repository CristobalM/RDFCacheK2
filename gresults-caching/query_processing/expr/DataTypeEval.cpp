//
// Created by cristobal on 4/20/21.
//

#include "DataTypeEval.hpp"
#include "DataTypeResource.hpp"
std::unique_ptr<TermResource>
DataTypeEval::eval_resource(const ExprEval::row_t &row) {
  auto resource = children[0]->eval_resource(row);
  if(!resource->is_concrete()){
    this->with_error = true;
    return TermResource::null();
  }
  const auto &concrete_resource = resource->get_resource();
  if(concrete_resource.resource_type != RDFResourceType::RDF_TYPE_LITERAL){
    this->with_error = true;
    return TermResource::null();
  }
  auto datatype = persistent_data.extract_data_type_from_string(concrete_resource.value);
  return std::make_unique<DataTypeResource>(datatype);
}

void DataTypeEval::validate() {
  ExprEval::validate();
  assert_fsize(1);
  assert_is_function(expr_node);
}
void DataTypeEval::init() {
  ExprEval::init();
  add_children();
}
/*
RDFResource DataTypeEval::resource_from_datatype(ExprDataType type) {
  switch (type) {
  case EDT_BOOLEAN:
    return RDFResource("<http://www.w3.org/2001/XMLSchema#boolean>", RDFResourceType::RDF_TYPE_IRI);
  case EDT_INTEGER:
    return RDFResource("<http://www.w3.org/2001/XMLSchema#integer>", RDFResourceType::RDF_TYPE_IRI);
  case EDT_DECIMAL:
    return RDFResource("<http://www.w3.org/2001/XMLSchema#decimal>", RDFResourceType::RDF_TYPE_IRI);
  case EDT_FLOAT:
    return RDFResource("<http://www.w3.org/2001/XMLSchema#float>", RDFResourceType::RDF_TYPE_IRI);
  case EDT_DOUBLE:
    return RDFResource("<http://www.w3.org/2001/XMLSchema#double>", RDFResourceType::RDF_TYPE_IRI);
  case EDT_STRING:
    return RDFResource("<http://www.w3.org/2001/XMLSchema#string>", RDFResourceType::RDF_TYPE_IRI);
  case EDT_DATETIME:
    return RDFResource("<http://www.w3.org/2001/XMLSchema#dateTime>", RDFResourceType::RDF_TYPE_IRI);
  case EDT_UNKNOWN:
  default:
    return RDFResource::null_resource();
  }
}
*/