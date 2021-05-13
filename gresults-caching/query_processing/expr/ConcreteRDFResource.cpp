//
// Created by cristobal on 4/21/21.
//

#include "ConcreteRDFResource.hpp"
#include "StringHandlingUtil.hpp"
#include <query_processing/ExprProcessorPersistentData.hpp>
bool ConcreteRDFResource::operator==(const TermResource &rhs) const {
  return rhs.is_concrete() && resource == rhs.get_resource();
}

ConcreteRDFResource::ConcreteRDFResource(RDFResource &&resource)
    : resource(std::move(resource)) {}
ConcreteRDFResource::ConcreteRDFResource(const RDFResource &resource)
    : resource(resource) {}
bool ConcreteRDFResource::is_concrete() const { return true; }
const RDFResource &ConcreteRDFResource::get_resource() const {
  return resource;
}
ConcreteRDFResource ConcreteRDFResource::null_resource() {
  return ConcreteRDFResource(RDFResource::null_resource());
}
std::shared_ptr<TermResource> ConcreteRDFResource::null_resource_ptr() {
  return std::make_shared<ConcreteRDFResource>(RDFResource::null_resource());
}

RDFResource ConcreteRDFResource::get_resource_clone() const { return resource; }

bool ConcreteRDFResource::contains(TermResource &pattern_resource) const {
  auto data_type =
      ExprProcessorPersistentData::get().extract_data_type_from_string(
          resource.value);
  auto lang_tag =
      ExprProcessorPersistentData::get().extract_language_tag(resource.value);
  auto value =
      ExprProcessorPersistentData::get().extract_literal_content_from_string(
          resource.value);

  if (pattern_resource.is_string_literal()) {
    if (!lang_tag.empty() || data_type != pattern_resource.get_datatype())
      return false;
    return value.find(pattern_resource.get_literal_string()) !=
           std::string::npos;
  }
  if (pattern_resource.is_string_literal_lang()) {
    if (data_type != ExprDataType::EDT_UNKNOWN ||
        lang_tag != pattern_resource.get_lang_tag())
      return false;
    return value.find(pattern_resource.get_literal_lang_string()) !=
           std::string::npos;
  }
  if (pattern_resource.is_concrete()) {
    const auto &pattern_full_string = pattern_resource.get_resource().value;
    auto pattern_data_type =
        ExprProcessorPersistentData::get().extract_data_type_from_string(
            pattern_full_string);
    auto pattern_lang_tag =
        ExprProcessorPersistentData::get().extract_language_tag(
            pattern_full_string);
    if (pattern_data_type != data_type || pattern_lang_tag != lang_tag)
      return false;
    return value.find(pattern_full_string) != std::string::npos;
  }
  return false;
}
bool ConcreteRDFResource::is_literal() const {
  return resource.resource_type == RDFResourceType::RDF_TYPE_LITERAL;
}
const std::string &ConcreteRDFResource::get_literal_string() const {
  return TermResource::get_literal_string();
}
std::string ConcreteRDFResource::get_content_string_copy() const {
  StringLiteralData literal_data;
  switch (resource.resource_type) {
  case RDF_TYPE_IRI:
    return resource.value;
  case RDF_TYPE_BLANK:
    return resource.value;
  case RDF_TYPE_LITERAL:
    literal_data =
        StringHandlingUtil::extract_literal_data_from_rdf_resource(resource);
    return std::move(literal_data.value);
  case NULL_RESOURCE_TYPE:
    return "";
  }
  return "";
}
