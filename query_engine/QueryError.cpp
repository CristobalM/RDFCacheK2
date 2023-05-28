//
// Created by cristobal on 7/20/21.
//

#include "QueryError.hpp"
namespace k2cache {

QueryError::QueryError(std::string str_error)
    : str_error(std::move(str_error)) {}
std::string &QueryError::get_str() { return str_error; }
}
