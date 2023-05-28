//
// Created by cristobal on 26-06-22.
//

#ifndef RDFCACHEK2_NODESMAP_HPP
#define RDFCACHEK2_NODESMAP_HPP

#include <memory>

namespace k2cache {
class I_OStream;
struct NodesMap {
  virtual ~NodesMap() = default;

  virtual uint64_t get_id(uint64_t real_id) const = 0;
  virtual uint64_t get_real_id(uint64_t id, int *err_code) const = 0;

  virtual uint64_t get_last_assigned() = 0;
  virtual void add(uint64_t real_id, uint64_t mapped_id) = 0;

  virtual void serialize(I_OStream &os) = 0;

  virtual void restore(std::unique_ptr<I_IStream> &&log_istream,
                       std::unique_ptr<I_IStream> &&counter_istream) = 0;

  virtual int impl_id() = 0;
};

} // namespace k2cache

#endif // RDFCACHEK2_NODESMAP_HPP
