
//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_EXPR_DATATYPE_HPP
#define RDFCACHEK2_EXPR_DATATYPE_HPP

enum ExprDataType {
  EDT_UNKNOWN = -1,
  EDT_INTEGER = 0,
  EDT_DECIMAL = 1,
  EDT_FLOAT = 2,
  EDT_DOUBLE = 3,
  EDT_STRING = 4,
  EDT_BOOOLEAN = 5,
  EDT_DATETIME = 6
};

#endif