//
// Created by cristobal on 4/20/21.
//

#include <algorithm>

#include "BooleanResource.hpp"
#include "ConcreteRDFResource.hpp"
#include "DataTypeResource.hpp"
#include "DoubleResource.hpp"
#include "FloatResource.hpp"
#include "IntegerResource.hpp"
#include "StringLiteralLangResource.hpp"
#include "StringLiteralResource.hpp"
#include "TermEval.hpp"
RDFResource TermEval::eval_term_node(const ExprEval::row_t &row) const {
  switch (expr_node.term_node().term_type()) {
  case proto_msg::TermType::VARIABLE:
    return eval_variable_get_resource(row);
  case proto_msg::TermType::LITERAL:
  case proto_msg::TermType::BLANK_NODE:
  case proto_msg::TermType::IRI:
    return RDFResource(expr_node.term_node());
  default:
    throw std::runtime_error("Unexpected term type");
  }
}

RDFResource
TermEval::eval_variable_get_resource(const ExprEval::row_t &row) const {
  const auto &term = this->expr_node.term_node();
  if (term.term_type() != proto_msg::TermType::VARIABLE)
    throw std::runtime_error("Expected a variable");
  const std::string &var_name = term.term_value();
  if (this->eval_data.var_pos_mapping.find(var_name) ==
      this->eval_data.var_pos_mapping.end())
    throw std::runtime_error("Variable " + var_name + " not in table");
  auto pos = this->eval_data.var_pos_mapping.at(var_name);
  auto value_id = row[pos];
  auto resource = this->eval_data.cm.extract_resource(value_id);
  return resource;
}

void TermEval::validate() {
  ExprEval::validate();
  if (expr_node.expr_case() != proto_msg::ExprNode::kTermNode)
    throw std::runtime_error("Expected a term");
}
std::unique_ptr<TermResource>
TermEval::eval_resource(const ExprEval::row_t &row) {
  auto resource = eval_term_node(row);
  auto data_type =
      ExprProcessorPersistentData::get().extract_data_type_from_string(
          resource.value);
  auto content =
      ExprProcessorPersistentData::get().extract_literal_content_from_string(
          resource.value);
  if (data_type != EDT_UNKNOWN) {
    return make_data_type_resource(std::move(content), data_type);
  }

  auto lang_tag =
      ExprProcessorPersistentData::get().extract_language_tag(resource.value);
  if (!lang_tag.empty()) {
    return std::make_unique<StringLiteralLangResource>(std::move(content),
                                                       std::move(lang_tag));
  }

  return std::make_unique<StringLiteralResource>(std::move(content),
                                                 EDT_UNKNOWN);
}

bool TermEval::eval_boolean(const ExprEval::row_t &row) {
  const auto &term = this->expr_node.term_node();
  switch (term.term_type()) {
  case proto_msg::TermType::VARIABLE:
    return eval_boolean_from_resource(eval_variable_get_resource(row));
  case proto_msg::TermType::LITERAL:
    return eval_boolean_from_string(
        ExprProcessorPersistentData::get().extract_literal_content_from_string(
            term.term_value()));
  case proto_msg::TermType::BLANK_NODE:
  case proto_msg::TermType::IRI:
  default:
    throw std::runtime_error("Unexpected term type");
  }
}

int TermEval::eval_integer(const ExprEval::row_t &row) {
  const auto &term = this->expr_node.term_node();
  switch (term.term_type()) {
  case proto_msg::TermType::VARIABLE:
    return eval_integer_from_resource(eval_variable_get_resource(row));
  case proto_msg::TermType::LITERAL:
    return eval_integer_from_string(
        ExprProcessorPersistentData::get().extract_literal_content_from_string(
            term.term_value()));
  case proto_msg::TermType::BLANK_NODE:
  case proto_msg::TermType::IRI:
  default:
    throw std::runtime_error("Unexpected term type");
  }
}
float TermEval::eval_float(const ExprEval::row_t &row) {
  const auto &term = this->expr_node.term_node();
  switch (term.term_type()) {
  case proto_msg::TermType::VARIABLE:
    return eval_float_from_resource(eval_variable_get_resource(row));
  case proto_msg::TermType::LITERAL:
    return eval_float_from_string(
        ExprProcessorPersistentData::get().extract_literal_content_from_string(
            term.term_value()));
  case proto_msg::TermType::BLANK_NODE:
  case proto_msg::TermType::IRI:
  default:
    throw std::runtime_error("Unexpected term type");
  }
}
double TermEval::eval_double(const ExprEval::row_t &row) {
  const auto &term = this->expr_node.term_node();
  switch (term.term_type()) {
  case proto_msg::TermType::VARIABLE:
    return eval_double_from_resource(eval_variable_get_resource(row));
  case proto_msg::TermType::LITERAL:
    return eval_double_from_string(
        ExprProcessorPersistentData::get().extract_literal_content_from_string(
            term.term_value()));
  case proto_msg::TermType::BLANK_NODE:
  case proto_msg::TermType::IRI:
  default:
    throw std::runtime_error("Unexpected term type");
  }
}
void TermEval::init() { ExprEval::init(); }
bool TermEval::eval_boolean_from_resource(const RDFResource &resource) {
  if (resource.resource_type != RDFResourceType::RDF_TYPE_LITERAL)
    throw std::runtime_error("Unexpected term type");
  return eval_boolean_from_string(resource.value);
}
bool TermEval::eval_boolean_from_string(const std::string &input_string) {
  auto parsed_string =
      ExprProcessorPersistentData::get().extract_literal_content_from_string(
          input_string);
  std::for_each(parsed_string.begin(), parsed_string.end(),
                [](char &c) { c = std::tolower(c); });
  if (parsed_string == "false")
    return false;
  return true;
}

int TermEval::eval_integer_from_resource(const RDFResource &resource) {
  if (resource.resource_type != RDFResourceType::RDF_TYPE_LITERAL)
    throw std::runtime_error("Unexpected term type");
  return eval_integer_from_string(resource.value);
}

int TermEval::eval_integer_from_string(const std::string &input_string) {
  auto parsed_string =
      ExprProcessorPersistentData::get().extract_literal_content_from_string(
          input_string);
  if (!ExprProcessorPersistentData::get().string_is_numeric(parsed_string)) {
    this->with_error = true;
    return 0;
  }
  return std::stoi(parsed_string);
}
float TermEval::eval_float_from_resource(const RDFResource &resource) {
  if (resource.resource_type != RDFResourceType::RDF_TYPE_LITERAL)
    throw std::runtime_error("Unexpected term type");
  return eval_float_from_string(resource.value);
}

float TermEval::eval_float_from_string(const std::string &input_string) {
  auto parsed_string =
      ExprProcessorPersistentData::get().extract_literal_content_from_string(
          input_string);
  if (!ExprProcessorPersistentData::get().string_is_numeric(parsed_string)) {
    this->with_error = true;
    return 0;
  }
  return std::stof(parsed_string);
}
double TermEval::eval_double_from_resource(const RDFResource &resource) {
  if (resource.resource_type != RDFResourceType::RDF_TYPE_LITERAL)
    throw std::runtime_error("Unexpected term type");
  return eval_double_from_string(resource.value);
}

double TermEval::eval_double_from_string(const std::string &input_string) {
  auto parsed_string =
      ExprProcessorPersistentData::get().extract_literal_content_from_string(
          input_string);
  if (!ExprProcessorPersistentData::get().string_is_numeric(parsed_string)) {
    this->with_error = true;
    return 0;
  }
  return std::stod(parsed_string);
}

DateInfo TermEval::eval_date_time(const ExprEval::row_t &row) {

  static UErrorCode parser_err = U_ZERO_ERROR;
  static icu::UnicodeString pattern("YYYY-MM-DDThh:mm:ss.sTZD");
  static icu::SimpleDateFormat parser(pattern, parser_err);

  auto resource = eval_term_node(row);
  if (resource.resource_type != RDFResourceType::RDF_TYPE_LITERAL) {
    with_error = true;
    return DateInfo{};
  }

  const auto parsed_date_info =
      ExprProcessorPersistentData::get().parse_iso8601(resource.value);
  if (!parsed_date_info.matched) {
    with_error = true;
    return DateInfo{};
  }
  return parsed_date_info;
}

std::unique_ptr<TermResource>
TermEval::eval_datatype(const ExprEval::row_t &row) {
  auto resource = eval_resource(row);
  if (!resource->is_concrete()) {
    this->with_error = true;
    return TermResource::null();
  }
  const auto &concrete_resource = resource->get_resource();
  if (concrete_resource.resource_type != RDFResourceType::RDF_TYPE_LITERAL) {
    this->with_error = true;
    return TermResource::null();
  }
  auto datatype =
      ExprProcessorPersistentData::get().extract_data_type_from_string(
          concrete_resource.value);
  return std::make_unique<DataTypeResource>(datatype);
}
std::unique_ptr<TermResource>
TermEval::make_data_type_resource(std::string &&input_string,
                                  ExprDataType data_type) {
  switch (data_type) {
  case EDT_UNKNOWN:
    return TermResource::null();
  case EDT_INTEGER:
    return std::make_unique<IntegerResource>(std::stoi(input_string));
  case EDT_DECIMAL:
  case EDT_FLOAT:
    return std::make_unique<FloatResource>(std::stof(input_string));
  case EDT_DOUBLE:
    return std::make_unique<DoubleResource>(std::stod(input_string));

  case EDT_BOOLEAN:
    std::for_each(input_string.begin(), input_string.end(),
                  [](char &c) { c = std::tolower(c); });
    return std::make_unique<BooleanResource>(input_string != "false");
  case EDT_DATETIME:
  case EDT_STRING:
    return std::make_unique<StringLiteralResource>(std::move(input_string),
                                                   data_type);
  }
}
