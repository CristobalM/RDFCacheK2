#ifndef _PREDICATES_INDEX_FILE_BUILDER_HPP_
#define _PREDICATES_INDEX_FILE_BUILDER_HPP_

#include "k2tree/K2TreeMixed.hpp"
#include "manager/PredicatesCacheMetadata.hpp"
#include "TriplesFeedSortedByPredicate.hpp"
#include "K2TreesFeed.hpp"
#include <cstdint>
#include <istream>
#include <ostream>

namespace k2cache {
class PredicatesIndexFileBuilder {

public:
  static PredicatesCacheMetadata build(std::istream &input_file,
                                       std::ostream &output_file,
                                       std::iostream &tmp_stream,
                                       K2TreeConfig config);
  // Deprecated
  static PredicatesCacheMetadata build_orig(std::istream &input_file,
                                       std::ostream &output_file,
                                       std::iostream &tmp_stream,
                                       K2TreeConfig config);

  static PredicatesCacheMetadata build_with_feed(TriplesFeedSortedByPredicate &feed,
                                       std::ostream &output_file,
                                       std::iostream &tmp_stream,
                                       K2TreeConfig config);
  static PredicatesCacheMetadata build_with_k2tree_feed(K2TreesFeed &feed,
                                       std::ostream &output_file,
                                       std::iostream &tmp_stream,
                                       K2TreeConfig config);

  static PredicatesCacheMetadata
  build_debug(std::istream &input_file, std::ostream &output_file,
              std::iostream &tmp_stream, K2TreeConfig config, bool slow_check);
};
} // namespace k2cache

#endif /* _PREDICATES_INDEX_FILE_BUILDER_HPP_ */