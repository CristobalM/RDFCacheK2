//
// Created by cristobal on 4/29/21.
//

#ifndef RDFCACHEK2_STRINGHANDLINGUTIL_HPP
#define RDFCACHEK2_STRINGHANDLINGUTIL_HPP

#include "TermResource.hpp"
#include <query_processing/ExprDataType.hpp>
#include <string>

struct StringLiteralData {
  std::string value;
  ExprDataType type;
  std::string lang_tag;
  bool error;
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
