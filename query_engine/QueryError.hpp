//
// Created by cristobal on 7/20/21.
//

#ifndef RDFCACHEK2_QUERYERROR_HPP
#define RDFCACHEK2_QUERYERROR_HPP

#include <string>
namespace k2cache {

class QueryError {
  std::string str_error;

public:
  explicit QueryError(std::string str_error);
  std::string &get_str();
};

}

#endif // RDFCACHEK2_QUERYERROR_HPP
