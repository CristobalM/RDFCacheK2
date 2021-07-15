//
// Created by cristobal on 7/13/21.
//

#ifndef RDFCACHEK2_OptionalProcessor2_HPP
#define RDFCACHEK2_OptionalProcessor2_HPP

#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "LWRHMapJoinLazyBaseProcessor.hpp"
#include "QueryProcHashing.hpp"
#include "ResultTable.hpp"
#include "ResultTableIterator.hpp"
#include "VarIndexManager.hpp"

class OptionalProcessor2 : public LWRHMapJoinLazyBaseProcessor {

public:
  OptionalProcessor2(std::shared_ptr<ResultTableIterator> left_it,
                     std::shared_ptr<ResultTableIterator> right_it);
  std::shared_ptr<ResultTable> execute();
  std::shared_ptr<ResultTableIterator> execute_it();
};
#endif // RDFCACHEK2_OptionalProcessor2_HPP
