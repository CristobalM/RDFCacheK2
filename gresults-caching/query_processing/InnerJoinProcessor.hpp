//
// Created by cristobal on 15-07-21.
//

#ifndef RDFCACHEK2_INNERJOINPROCESSOR_HPP
#define RDFCACHEK2_INNERJOINPROCESSOR_HPP

#include "LWRHMapJoinLazyBaseProcessor.hpp"
#include <TimeControl.hpp>
#include <memory>

class InnerJoinProcessor : public LWRHMapJoinLazyBaseProcessor {
public:
  InnerJoinProcessor(std::shared_ptr<ResultTableIterator> left_it,
                     std::shared_ptr<ResultTableIterator> right_it,
                     TimeControl &time_control);

  std::shared_ptr<ResultTableIterator> execute_it();
  std::shared_ptr<ResultTable> execute();
};

#endif // RDFCACHEK2_INNERJOINPROCESSOR_HPP
