#include "ChainedInput.hpp"

ChainedInput::ChainedInput(ResultTable &result_table,
                           const std::vector<Triple> &triples,
                           const PredicatesCacheManager &cm,
                           const std::vector<coord_t> &coord_types)
    : result_table(result_table), triples(triples), cm(cm),
      coord_types(coord_types) {}