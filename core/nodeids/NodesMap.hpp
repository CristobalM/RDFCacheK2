//
// Created by cristobal on 26-06-22.
//

#ifndef RDFCACHEK2_NODESMAP_HPP
#define RDFCACHEK2_NODESMAP_HPP

namespace k2cache {
class I_OStream;
struct NodesMap {
  virtual ~NodesMap() = default;

  virtual long get_id(long real_id) = 0;

  virtual void serialize(I_OStream &os) = 0;

  virtual int impl_id() = 0;
};

} // namespace k2cache

#endif // RDFCACHEK2_NODESMAP_HPP
