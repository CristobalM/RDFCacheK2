//
// Created by cristobal on 24-05-22.
//

#ifndef RDFCACHEK2_UPDATELOGCOMPACTER_HPP
#define RDFCACHEK2_UPDATELOGCOMPACTER_HPP

#include "PredicatesIndexCacheMD.hpp"
#include "UpdatesLogger.hpp"
class UpdateLogCompacter {
  UpdatesLogger &updates_logger;
  PredicatesIndexCacheMD &predicates_index_cache;
public:
  UpdateLogCompacter(
      UpdatesLogger &updates_logger,
      PredicatesIndexCacheMD &predicates_index_cache
      );

  void compact();
};

#endif // RDFCACHEK2_UPDATELOGCOMPACTER_HPP
