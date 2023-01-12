//
// Created by Cristobal Miranda on 09-01-23.
//

#ifndef RDFCACHEK2_K2TREESFEEDFROMSORTEDTRIPLESFEED_HPP
#define RDFCACHEK2_K2TREESFEEDFROMSORTEDTRIPLESFEED_HPP

#include "K2TreesFeed.hpp"
#include "TriplesFeedSortedByPredicate.hpp"
namespace k2cache{

class K2TreesFeedFromSortedTriplesFeed : public K2TreesFeed {
  TriplesFeedSortedByPredicate &feed;
  K2TreeConfig config;
  std::unique_ptr<RDFTriple> carry_triple;
  uint64_t triple_cnt;
public:
  K2TreesFeedFromSortedTriplesFeed(TriplesFeedSortedByPredicate &feed,  K2TreeConfig config);
  bool has_next() override;
  K2TreeContainer get_next() override;
};

}

#endif // RDFCACHEK2_K2TREESFEEDFROMSORTEDTRIPLESFEED_HPP
