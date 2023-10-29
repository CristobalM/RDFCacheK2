//
// Created by cristobal on 28-05-23.
//

#ifndef RDFCACHEK2_BGPMESSAGE_HPP
#define RDFCACHEK2_BGPMESSAGE_HPP

#include <string>
#include <vector>

struct BGPNode {
  bool is_concrete; // if true, then value is real_node_id or var_name otherwise
  std::string var_name;
  uint64_t real_node_id;
  uint64_t cache_node_id = -1; // not necessarily filled at any time
};

struct BGPTriple {
  BGPNode subject;
  BGPNode predicate;
  BGPNode object;
};

struct BGPMessage {
  std::vector<std::string> var_names;
  std::vector<BGPTriple> patterns;
  bool first_batch_small;
};

#endif // RDFCACHEK2_BGPMESSAGE_HPP
