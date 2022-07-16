//
// Created by cristobal on 26-06-22.
//

#ifndef RDFCACHEK2_NODESMAPFACTORY_HPP
#define RDFCACHEK2_NODESMAPFACTORY_HPP
#include "I_IStream.hpp"
#include "NodesMap.hpp"
#include <memory>
namespace k2cache {
struct NodesMapFactory {
  static std::unique_ptr<NodesMap> from_input_stream(I_IStream &input_stream);
};
} // namespace k2cache

#endif // RDFCACHEK2_NODESMAPFACTORY_HPP
