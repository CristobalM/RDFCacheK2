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
  uint64_t cache_node_id = -1; // not necessarily filled at any time
};

struct BgpTriple {
  BgpNode subject;
  BgpNode predicate;
  BgpNode object;
};

struct BgpMessage {
  std::vector<std::string> var_names;
  std::vector<BgpTriple> patterns;
};

#endif // RDFCACHEK2_BGPMESSAGE_HPP
