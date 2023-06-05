//
// Created by cristobal on 05-06-23.
//

#ifndef RDFCACHEK2_BGP_TEST_UTIL_HPP
#define RDFCACHEK2_BGP_TEST_UTIL_HPP

#include "BGPMessage.hpp"
namespace k2cache {
  static BGPMessage create_simple_bgp_message(int var_num){
    BGPMessage msg;
    msg.var_names.reserve(var_num);
    for(int i = 0; i < var_num; ++i){
      msg.var_names.push_back("?x" + std::to_string(i));
    }
    auto real_ids = 123;
    auto cache_ids = 0;

    auto fill_var_bgp_node = [&](BGPNode &node, const std::string &varname) {
      node.real_node_id = ++real_ids;
      node.cache_node_id = ++cache_ids;
      node.is_concrete = false;
      node.var_name = varname;
    };
    bool curr = false;
    auto fill_bgp_triple = [&](BGPTriple &triple, const std::string &varname) {
      fill_var_bgp_node(triple.subject, varname);
      fill_var_bgp_node(triple.predicate, varname);
      fill_var_bgp_node(triple.object, varname);
      triple.predicate.is_concrete = true;
      if(curr){
        triple.subject.is_concrete = true;
      }
      else{
        triple.object.is_concrete = true;
      }
      curr = !curr;
    };

    std::vector<BGPTriple> triples;
    triples.reserve(msg.var_names.size());
    for(const auto& var_name : msg.var_names){
      BGPTriple triple{};
      fill_bgp_triple(triple, var_name);
      triples.push_back(triple);
    }

    msg.patterns = std::move(triples);
    return msg;
  }
}
#endif // RDFCACHEK2_BGP_TEST_UTIL_HPP
