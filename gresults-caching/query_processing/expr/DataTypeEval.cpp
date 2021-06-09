//
// Created by cristobal on 4/20/21.
//

#include "DataTypeEval.hpp"
#include "query_processing/resources/DataTypeResource.hpp"
std::shared_ptr<TermResource>
DataTypeEval::eval_resource(const ExprEval::row_t &row) {
  auto result = children[0]->eval_datatype(row);
  with_error = children[0]->has_error();
  return result;
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
    return RDFResource("<http://www.w3.org/2001/XMLSchema#boolean>",
RDFResourceType::RDF_TYPE_IRI); case EDT_INTEGER: return
RDFResource("<http://www.w3.org/2001/XMLSchema#integer>",
RDFResourceType::RDF_TYPE_IRI); case EDT_DECIMAL: return
RDFResource("<http://www.w3.org/2001/XMLSchema#decimal>",
RDFResourceType::RDF_TYPE_IRI); case EDT_FLOAT: return
RDFResource("<http://www.w3.org/2001/XMLSchema#float>",
RDFResourceType::RDF_TYPE_IRI); case EDT_DOUBLE: return
RDFResource("<http://www.w3.org/2001/XMLSchema#double>",
RDFResourceType::RDF_TYPE_IRI); case EDT_STRING: return
RDFResource("<http://www.w3.org/2001/XMLSchema#string>",
RDFResourceType::RDF_TYPE_IRI); case EDT_DATETIME: return
RDFResource("<http://www.w3.org/2001/XMLSchema#dateTime>",
RDFResourceType::RDF_TYPE_IRI); case EDT_UNKNOWN: default: return
RDFResource::null_resource();
  }
}
*/