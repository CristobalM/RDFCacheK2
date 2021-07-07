//
// Created by cristobal on 4/20/21.
//

#include "StrConcatEval.hpp"
#include "query_processing/resources/StringLiteralLangResource.hpp"
#include "query_processing/resources/StringLiteralResource.hpp"

#include <sstream>
#include <vector>

std::shared_ptr<TermResource>
StrConcatEval::eval_resource(const ExprEval::row_t &row) {
  std::vector<std::shared_ptr<TermResource>> resources;
  for (const auto &child_ptr : children) {
    resources.push_back(child_ptr->produce_resource(row));
    if (child_ptr->has_error()) {
      this->with_error = true;
      return TermResource::null();
    }
  }
  std::stringstream ss;
  ExprDataType data_type = ExprDataType::EDT_UNKNOWN;
  std::string lang_tag;
  bool wlang_tag = false;
  bool wdata_type = false;
  bool first = true;
  for (const auto &resource : resources) {
    if (resource->is_string_literal_lang()) {
      ss << resource->get_literal_lang_string();
      if (first) {
        lang_tag = resource->get_lang_tag();
        first = false;
        wlang_tag = true;
      } else if (wlang_tag && resource->get_lang_tag() != lang_tag) {
        wlang_tag = false;
      }
      wdata_type = false;
    } else if (resource->is_string_literal()) {
      ss << resource->get_literal_string();
      if (first) {
        data_type = resource->get_datatype();
        first = false;
        wdata_type = true;
      } else if (wdata_type && resource->get_datatype() != data_type) {
        wdata_type = false;
      }
      wlang_tag = false;
    } else if (resource->is_concrete()) {
      ss << resource->get_content_string_copy();
      auto concrete_dtype =
          ExprProcessorPersistentData::get().extract_data_type_from_string(
              resource->get_resource().value);
      auto concrete_ltag =
          ExprProcessorPersistentData::get().extract_language_tag(
              resource->get_resource().value);
      if (first) {
        if (concrete_dtype != ExprDataType::EDT_UNKNOWN) {
          data_type = concrete_dtype;
          wdata_type = true;
        } else if (!concrete_ltag.empty()) {
          lang_tag = concrete_ltag;
          wlang_tag = true;
        }
        first = false;
      } else {
        if (wdata_type && concrete_dtype != ExprDataType::EDT_UNKNOWN &&
            data_type != concrete_dtype) {
          wdata_type = false;
        } else if (wlang_tag && !concrete_ltag.empty() &&
                   lang_tag != concrete_ltag) {
          wlang_tag = false;
        }

        if (concrete_dtype != ExprDataType::EDT_UNKNOWN)
          wlang_tag = false;
        if (!concrete_ltag.empty())
          wdata_type = false;
      }
    } else if (resource->can_cast_to_literal_string()) {
      ss << resource->get_content_string_copy();
    } else {
      this->with_error = true;
      return TermResource::null();
    }
  }
  if (wlang_tag) {
    return std::make_shared<StringLiteralLangResource>(ss.str(),
                                                       std::move(lang_tag));
  }
  if (wdata_type) {
    ExprDataType resulting_data_type =
        (data_type == EDT_STRING) ? EDT_STRING : EDT_UNKNOWN;
    return std::make_shared<StringLiteralResource>(ss.str(),
                                                   resulting_data_type);
  }

  return std::make_shared<StringLiteralResource>(ss.str(),
                                                 ExprDataType::EDT_UNKNOWN);
}
void StrConcatEval::validate() {
  ExprEval::validate();
  // variable number of children
}
void StrConcatEval::init() {
  ExprEval::init();
  add_children();
}
