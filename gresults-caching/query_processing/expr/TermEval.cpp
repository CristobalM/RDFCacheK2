//
// Created by cristobal on 4/20/21.
//

#include <algorithm>

#include "TermEval.hpp"
#include "query_processing/resources/BooleanResource.hpp"
#include "query_processing/resources/ConcreteRDFResource.hpp"
#include "query_processing/resources/DataTypeResource.hpp"
#include "query_processing/resources/DateTimeResource.hpp"
#include "query_processing/resources/DoubleResource.hpp"
#include "query_processing/resources/FloatResource.hpp"
#include "query_processing/resources/IntegerResource.hpp"
#include "query_processing/resources/StringLiteralLangResource.hpp"
#include "query_processing/resources/StringLiteralResource.hpp"
#include "query_processing/utility/StringHandlingUtil.hpp"
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
  auto last_cache_id = this->eval_data.cm.get_last_id();
  if (value_id > last_cache_id)
    return this->eval_data.extra_dict.extract_resource(value_id -
                                                       last_cache_id);
  return this->eval_data.cm.extract_resource(value_id);
}

void TermEval::validate() {
  ExprEval::validate();
  if (expr_node.expr_case() != proto_msg::ExprNode::kTermNode)
    throw std::runtime_error("Expected a term");
}
std::shared_ptr<TermResource>
TermEval::eval_resource(const ExprEval::row_t &row) {
  auto resource = eval_term_node(row);

  if (resource.resource_type == RDFResourceType::RDF_TYPE_IRI) {
    return eval_iri_resource(std::move(resource));
  } else if (resource.resource_type == RDFResourceType::RDF_TYPE_BLANK) {
    return std::make_shared<ConcreteRDFResource>(std::move(resource));
  }

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
    return std::make_shared<StringLiteralLangResource>(std::move(content),
                                                       std::move(lang_tag));
  }

  return std::make_shared<StringLiteralResource>(std::move(content),
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
    return eval_integer_from_string(term.term_value());
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
void TermEval::init() {
  ExprEval::init();
  with_constant_subtree = has_constant_subtree();
}
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

  auto literal_content =
      StringHandlingUtil::extract_literal_data_from_rdf_resource(resource);

  const auto parsed_date_info =
      ExprProcessorPersistentData::get().parse_iso8601(literal_content.value);
  if (!parsed_date_info.matched) {
    with_error = true;
    return DateInfo{};
  }
  return parsed_date_info;
}

std::shared_ptr<TermResource>
TermEval::eval_datatype(const ExprEval::row_t &row) {
  auto resource = eval_resource(row);
  return std::make_shared<DataTypeResource>(resource->get_datatype());
}

std::shared_ptr<TermResource>
TermEval::make_data_type_resource(std::string &&input_string,
                                  ExprDataType data_type) {
  switch (data_type) {
  case EDT_UNKNOWN:
    return TermResource::null();
  case EDT_INTEGER:
    return std::make_shared<IntegerResource>(std::stoi(input_string));
  case EDT_DECIMAL:
  case EDT_FLOAT:
    return std::make_shared<FloatResource>(std::stof(input_string));
  case EDT_DOUBLE:
    return std::make_shared<DoubleResource>(std::stod(input_string));

  case EDT_BOOLEAN:
    std::for_each(input_string.begin(), input_string.end(),
                  [](char &c) { c = std::tolower(c); });
    return std::make_shared<BooleanResource>(input_string != "false");
  case EDT_DATETIME:
    return std::make_shared<DateTimeResource>(
        ExprProcessorPersistentData::get().parse_iso8601(input_string));
  case EDT_STRING:
    return std::make_shared<StringLiteralResource>(std::move(input_string),
                                                   data_type);
  }
  return TermResource::null();
}
std::shared_ptr<TermResource>
TermEval::eval_iri_resource(RDFResource &&resource) {

  auto matches_short = StringHandlingUtil::starts_with(
      resource.value, DataTypeResource::short_prefix);
  auto matches_long = StringHandlingUtil::starts_with(
      resource.value, DataTypeResource::long_prefix);

  if (matches_short || matches_long) {
    return create_datatype_resource(std::move(resource), matches_short);
  }

  return std::make_shared<ConcreteRDFResource>(std::move(resource));
}

std::shared_ptr<TermResource>
TermEval::create_datatype_resource(RDFResource &&resource, bool matches_short) {
  std::string type_s;

  if (matches_short) {
    type_s = resource.value.substr(DataTypeResource::short_prefix.size());
  } else {
    // ignore the last character ">"
    type_s = resource.value.substr(DataTypeResource::long_prefix.size(),
                                   resource.value.size() - 1);
  }
  std::for_each(type_s.begin(), type_s.end(),
                [](char &c) { c = std::tolower(c); });
  if (type_s == "integer") {
    return DataTypeResource::create(ExprDataType::EDT_INTEGER);
  } else if (type_s == "string") {
    return DataTypeResource::create(ExprDataType::EDT_STRING);
  } else if (type_s == "float") {
    return DataTypeResource::create(ExprDataType::EDT_FLOAT);
  } else if (type_s == "double") {
    return DataTypeResource::create(ExprDataType::EDT_DOUBLE);
  } else if (type_s == "decimal") {
    return DataTypeResource::create(ExprDataType::EDT_DECIMAL);
  } else if (type_s == "datetime") {
    return DataTypeResource::create(ExprDataType::EDT_DATETIME);
  } else if (type_s == "boolean") {
    return DataTypeResource::create(ExprDataType::EDT_BOOLEAN);
  }

  return DataTypeResource::create(ExprDataType::EDT_UNKNOWN);
}
bool TermEval::has_constant_subtree() {
  return expr_node.term_node().term_type() != proto_msg::TermType::VARIABLE;
}
