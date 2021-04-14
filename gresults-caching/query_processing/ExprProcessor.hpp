
//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_EXPR_PROCESSOR_HPP
#define RDFCACHEK2_EXPR_PROCESSOR_HPP

#include <memory>

#include "ResultTable.hpp"
#include <request_msg.pb.h>

class ExprProcessor {
  std::shared_ptr<ResultTable> table;
  const proto_msg::ExprNode &expr_node;

public:
  ExprProcessor(std::shared_ptr<ResultTable> table,
                const proto_msg::ExprNode &expr_node);

  std::shared_ptr<ResultTable> execute();
};

#endif /* RDFCACHEK2_EXPR_PROCESSOR_HPP */