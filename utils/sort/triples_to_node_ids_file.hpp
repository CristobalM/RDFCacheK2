//
// Created by cristobal on 25-11-22.
//

#ifndef RDFCACHEK2_TRIPLES_TO_NODE_IDS_HPP
#define RDFCACHEK2_TRIPLES_TO_NODE_IDS_HPP
#include <string>
namespace k2cache{

struct TriplesToNodeIdsSortParams{
  std::string input_file;
  std::string output_file;
  int workers;
  unsigned long memory_budget;
};

/**
 * Produces a binary file with unique sorted nodeids from a file with triples
 * of nodeids
 * @param params
 * @return err_code
 * 0: successful
 * 1: input file doesn't exist
 */
int triples_to_sorted_node_ids(const TriplesToNodeIdsSortParams &params);

}
#endif // RDFCACHEK2_TRIPLES_TO_NODE_IDS_HPP
