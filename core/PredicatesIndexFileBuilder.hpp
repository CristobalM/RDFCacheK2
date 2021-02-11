#ifndef _PREDICATES_INDEX_FILE_BUILDER_HPP_
#define _PREDICATES_INDEX_FILE_BUILDER_HPP_

#include <istream>
#include <ostream>
#include <cstdint>
#include "PredicatesCacheMetadata.hpp"
#include "K2TreeMixed.hpp"

class PredicatesIndexFileBuilder {

public:

  
  static PredicatesCacheMetadata build(std::istream &input_file,
                             std::ostream &output_file, 
                            std::iostream &tmp_stream,
                             uint32_t treedepth,
                             uint32_t max_node_count, uint32_t cut_depth);

  
  static PredicatesCacheMetadata build(std::istream &input_file,
                             std::ostream &output_file, 
                            std::iostream &tmp_stream,
                             K2TreeConfig config);


  /*
  static PredicatesCacheMetadata build_from_memory(std::vector<TripleValue> &triples,
  std::ostream &output_file, 
                            std::iostream &tmp_stream,
                             K2TreeConfig config);

                             */
};

#endif /* _PREDICATES_INDEX_FILE_BUILDER_HPP_ */