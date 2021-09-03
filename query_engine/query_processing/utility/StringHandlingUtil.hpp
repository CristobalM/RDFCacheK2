//
// Created by cristobal on 4/29/21.
//

#ifndef RDFCACHEK2_STRINGHANDLINGUTIL_HPP
#define RDFCACHEK2_STRINGHANDLINGUTIL_HPP

#include "query_processing/resources/TermResource.hpp"
#include <query_processing/ExprDataType.hpp>
#include <string>

struct StringLiteralData {
  std::string value;
  ExprDataType type;
  std::string lang_tag;
  bool error;

  bool has_type() { return type != ExprDataType::EDT_UNKNOWN; }
  bool has_lang_tag() { return !lang_tag.empty(); }
  std::string type_to_str() {
    switch (type) {
    case EDT_INTEGER:
      return "integer";
    case EDT_DECIMAL:
      return "decimal";
    case EDT_FLOAT:
      return "float";
    case EDT_DOUBLE:
      return "double";
    case EDT_BOOLEAN:
      return "boolean";
    case EDT_DATETIME:
      return "dateTime";
    case EDT_UNKNOWN:
    case EDT_STRING:
      return "string";
    case EDT_WKT_LITERAL:
      return "wktLiteral";
    case EDT_GML_LITERAL:
      return "gmlLiteral";
    }
    return "string";
  }
};

class StringHandlingUtil {
public:
  static StringLiteralData
  extract_literal_data_from_term_resource(const TermResource &term_resource);
  static StringLiteralData
  extract_literal_data_from_rdf_resource(const RDFResource &rdf_resource);
  static StringLiteralData
  extract_literal_data_from_string(const std::string &input_string);

  static int regex_flag_options_from_string(const std::string &input_flags);

  static bool starts_with(const std::string &input, const std::string &pattern);
  static bool ends_with(const std::string &input, const std::string &pattern);
};

#endif // RDFCACHEK2_STRINGHANDLINGUTIL_HPP
