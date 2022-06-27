//
// Created by cristobal on 26-06-22.
//

#include "NodesMapImpl.hpp"
#include "node_ids_constants.hpp"
#include "serialization_util.hpp"
namespace k2cache {

NodesMapImpl::NodesMapImpl(std::map<long, long> &&imap)
    : imap(std::move(imap)) {
  for (auto p : this->imap) {
    rev_map[p.second] = p.first;
  }
}
void NodesMapImpl::serialize(I_OStream &os) {
  auto &osr = os.get_ostream();
  write_u32(osr, impl_id());
  write_u64(osr, imap.size());
  for (auto &p : imap) {
    write_u64(osr, p.first);
    write_u64(osr, p.second);
  }
}
int NodesMapImpl::impl_id() { return STD_MAP_NI_IMPL; }
long NodesMapImpl::get_id(long real_id) {
  auto it = imap.find(real_id);
  if (it == imap.end())
    return NOT_FOUND_NODEID;
  return it->second;
}
long NodesMapImpl::get_real_id(long id) { return rev_map[id]; }
} // namespace k2cache
