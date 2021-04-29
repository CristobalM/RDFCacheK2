//
// Created by cristobal on 4/21/21.
//

#include "ConcreteRDFResource.hpp"
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
std::unique_ptr<TermResource> ConcreteRDFResource::null_resource_ptr() {
  return std::make_unique<ConcreteRDFResource>(RDFResource::null_resource());
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
