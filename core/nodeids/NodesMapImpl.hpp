//
// Created by cristobal on 26-06-22.
//

#ifndef RDFCACHEK2_NODESMAPIMPL_HPP
#define RDFCACHEK2_NODESMAPIMPL_HPP
#include "I_IStream.hpp"
#include "I_OStream.hpp"
#include "NodesMap.hpp"
#include <map>
#include <memory>
namespace k2cache {
class NodesMapImpl : public NodesMap {
  std::map<uint64_t, uint64_t> imap;
  std::map<uint64_t, uint64_t> rev_map;

public:
  explicit NodesMapImpl(std::map<uint64_t, uint64_t> &&imap);
  void serialize(I_OStream &os) override;
  int impl_id() override;
  uint64_t get_id(uint64_t real_id) const override;
  uint64_t get_real_id(uint64_t id, int *err_code) const override;
  uint64_t get_last_assigned() override;
  void add(uint64_t real_id, uint64_t mapped_id) override;
  void restore(std::unique_ptr<I_IStream> &&log_istream,
               std::unique_ptr<I_IStream> &&counter_istream) override;
};
} // namespace k2cache
#endif // RDFCACHEK2_NODESMAPIMPL_HPP
