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
  std::map<long, long> imap;
  std::map<long, long> rev_map;

public:
  explicit NodesMapImpl(std::map<long, long> &&imap);
  void serialize(I_OStream &os) override;
  int impl_id() override;
  long get_id(long real_id) override;
  long get_real_id(long id, int *err_code) override;
  long get_last_assigned() override;
  void add(long real_id, long mapped_id) override;
  void restore(std::unique_ptr<I_IStream> &&log_istream,
               std::unique_ptr<I_IStream> &&counter_istream) override;
};
} // namespace k2cache
#endif // RDFCACHEK2_NODESMAPIMPL_HPP
