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
long NodesMapImpl::get_last_assigned() {
  if(rev_map.empty()) return NOT_FOUND_NODEID;
  return rev_map.rbegin()->first;
}
void NodesMapImpl::add(long real_id, long mapped_id) {
  imap[real_id] = mapped_id;
  rev_map[mapped_id] = real_id;
}
void NodesMapImpl::restore(std::unique_ptr<I_IStream> &&log_istream,
                           std::unique_ptr<I_IStream> &&counter_istream) {
  auto &counter_is = counter_istream->get_istream();
  auto kvs_size = (long)read_u64(counter_is);
  auto &is = log_istream->get_istream();
  for(auto i = 0L; i < kvs_size; i++){
    auto key = (long)read_u64(is);
    auto value = (long)read_u64(is);
    add(key, value);
  }
}
} // namespace k2cache
