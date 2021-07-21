//
// Created by cristobal on 7/13/21.
//

#ifndef RDFCACHEK2_OptionalProcessor_HPP
#define RDFCACHEK2_OptionalProcessor_HPP

#include <TimeControl.hpp>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "LWRHMapJoinLazyBaseProcessor.hpp"
#include "QueryProcHashing.hpp"
#include "ResultTable.hpp"
#include "ResultTableIterator.hpp"
#include "VarIndexManager.hpp"

class OptionalProcessor : public LWRHMapJoinLazyBaseProcessor {
public:
  OptionalProcessor(std::shared_ptr<ResultTableIterator> left_it,
                    std::shared_ptr<ResultTableIterator> right_it,
                    TimeControl &time_control);
  std::shared_ptr<ResultTable> execute();
  std::shared_ptr<ResultTableIterator> execute_it();
};
#endif // RDFCACHEK2_OptionalProcessor_HPP
