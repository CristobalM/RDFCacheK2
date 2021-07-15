//
// Created by cristobal on 15-07-21.
//

#ifndef RDFCACHEK2_INNERJOINPROCESSOR2_HPP
#define RDFCACHEK2_INNERJOINPROCESSOR2_HPP

#include "LWRHMapJoinLazyBaseProcessor.hpp"
#include <memory>

class InnerJoinProcessor2 : public LWRHMapJoinLazyBaseProcessor {
public:
  InnerJoinProcessor2(std::shared_ptr<ResultTableIterator> left_it,
                      std::shared_ptr<ResultTableIterator> right_it);

  std::shared_ptr<ResultTableIterator> execute_it();
  std::shared_ptr<ResultTable> execute();
};

#endif // RDFCACHEK2_INNERJOINPROCESSOR2_HPP
