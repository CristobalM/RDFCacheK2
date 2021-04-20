

//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_EXPR_PROCESSOR_PERSISTENT_DATA_HPP
#define RDFCACHEK2_EXPR_PROCESSOR_PERSISTENT_DATA_HPP

#include <pcrecpp.h>
#include <string>

#include "ExprDataType.hpp"

class ExprProcessorPersistentData {
  const pcrecpp::RE re_datatype;

public:
  ExprProcessorPersistentData();
  ExprDataType
  extract_data_type_from_string(const std::string &input_string) const;

private:
  ExprDataType select_data_type(const std::string &data_type_string) const;
};

#endif