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
                             K2TreeConfig config);

};

#endif /* _PREDICATES_INDEX_FILE_BUILDER_HPP_ */