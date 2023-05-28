//
// Created by cristobal on 28-05-23.
//

#ifndef RDFCACHEK2_BGPMESSAGE_HPP
#define RDFCACHEK2_BGPMESSAGE_HPP

#include <string>
#include <vector>

struct BgpNode {
  bool is_concrete; // if true, then value is real_node_id or var_name otherwise
  std::string var_name;
  uint64_t real_node_id;
};

struct BgpTriple {
  BgpNode subject;
  BgpNode predicate;
  BgpNode object;
};

struct BgpMessage {
  std::vector<BgpTriple> patterns;
};

#endif // RDFCACHEK2_BGPMESSAGE_HPP
