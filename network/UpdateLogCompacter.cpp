//
// Created by cristobal on 24-05-22.
//

#include "UpdateLogCompacter.hpp"
UpdateLogCompacter::UpdateLogCompacter(
    UpdatesLogger &updates_logger,
    PredicatesIndexCacheMD &predicates_index_cache)
    : updates_logger(updates_logger),
      predicates_index_cache(predicates_index_cache) {

}
void UpdateLogCompacter::compact() {

}
