//
// Created by Cristobal Miranda on 09-01-23.
//

#include <cstdint>

#include "K2TreesFeedFromSortedTriplesFeed.hpp"
#include "k2tree/K2TreeBulkOp.hpp"

namespace k2cache {

bool K2TreesFeedFromSortedTriplesFeed::has_next() { return triple_cnt < feed.triples_number(); }
K2TreeContainer K2TreesFeedFromSortedTriplesFeed::get_next() {
  if(!has_next()){
    return {};
  }

  K2TreeContainer container;
  container.tree = std::make_unique<K2TreeMixed>(config);

  uint64_t last_predicate = -1;
  bool first = true;
  K2TreeBulkOp op(*container.tree);

  if(carry_triple){
    first = false;
    last_predicate = carry_triple->predicate;
    container.predicate = carry_triple->predicate;
    op.insert(carry_triple->subject, carry_triple->object);
    triple_cnt++;
  }
  while (feed.has_next()) {
    auto triple = feed.get_next();

    if(first){
      first = false;
    }
    else if(last_predicate != triple.predicate){
      if(!carry_triple){
        carry_triple = std::make_unique<RDFTriple>();
      }
      carry_triple->subject = triple.subject;
      carry_triple->predicate = triple.predicate;
      carry_triple->object = triple.object;
      break;
    }
    last_predicate = triple.predicate;
    container.predicate = triple.predicate;
    op.insert(triple.subject, triple.object);
    triple_cnt++;
  }

  return container;
}
K2TreesFeedFromSortedTriplesFeed::K2TreesFeedFromSortedTriplesFeed(TriplesFeedSortedByPredicate &feed, K2TreeConfig config)
    : feed(feed), config(config), carry_triple(nullptr), triple_cnt(0) {
}
} // namespace k2cache