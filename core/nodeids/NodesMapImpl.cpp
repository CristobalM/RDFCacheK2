//
// Created by cristobal on 26-06-22.
//

#include "NodesMapImpl.hpp"
#include "node_ids_constants.hpp"
#include "serialization_util.hpp"
namespace k2cache {

NodesMapImpl::NodesMapImpl(std::map<uint64_t, uint64_t> &&imap)
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
uint64_t NodesMapImpl::get_id(uint64_t real_id) {
  auto it = imap.find(real_id);
  if (it == imap.end())
    return NOT_FOUND_NODEID;
  return it->second;
}
uint64_t NodesMapImpl::get_real_id(uint64_t id, int *err_code) {
  auto it = rev_map.find(id);
  if(it == rev_map.end()){
    if(err_code != nullptr){
      *err_code = (int)NidsErrCode::NOT_FOUND_ERR_CODE;
    }
    return -1;
  }
  return it->second;
}
uint64_t NodesMapImpl::get_last_assigned() {
  if (rev_map.empty())
    return NOT_FOUND_NODEID;
  return rev_map.rbegin()->first;
}
void NodesMapImpl::add(uint64_t real_id, uint64_t mapped_id) {
  imap[real_id] = mapped_id;
  rev_map[mapped_id] = real_id;
}
void NodesMapImpl::restore(std::unique_ptr<I_IStream> &&log_istream,
                           std::unique_ptr<I_IStream> &&counter_istream) {
  auto &counter_is = counter_istream->get_istream();
  auto kvs_size = read_u64(counter_is);
  auto &is = log_istream->get_istream();
  for (uint64_t i = 0; i < kvs_size; i++) {
    auto key = read_u64(is);
    auto value = read_u64(is);
    add(key, value);
  }
}
} // namespace k2cache
