#ifndef _PREDICATES_INDEX_FILE_BUILDER_HPP_
#define _PREDICATES_INDEX_FILE_BUILDER_HPP_

#include "K2TreeMixed.hpp"
#include "PredicatesCacheMetadata.hpp"
#include <cstdint>
#include <istream>
#include <ostream>

class PredicatesIndexFileBuilder {

public:
  static PredicatesCacheMetadata build(std::istream &input_file,
                                       std::ostream &output_file,
                                       std::iostream &tmp_stream,
                                       K2TreeConfig config);
  static PredicatesCacheMetadata
  build_debug(std::istream &input_file, std::ostream &output_file,
              std::iostream &tmp_stream, K2TreeConfig config, bool slow_check);
};

#endif /* _PREDICATES_INDEX_FILE_BUILDER_HPP_ */