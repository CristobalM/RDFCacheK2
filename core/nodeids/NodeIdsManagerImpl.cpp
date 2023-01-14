//
// Created by cristobal on 26-06-22.
//

#include "NodeIdsManagerImpl.hpp"
#include "FileRWHandler.hpp"
#include "I_IStream.hpp"
#include "NodesMapFactory.hpp"
#include "NodesSequence.hpp"
#include "node_ids_constants.hpp"
#include "serialization_util.hpp"

namespace k2cache {
NodeIdsManagerImpl::NodeIdsManagerImpl(
    std::unique_ptr<I_FileRWHandler> &&plain_ni_fh,
    std::unique_ptr<I_FileRWHandler> &&mapped_ni_fh,
    std::unique_ptr<I_FileRWHandler> &&log_fh,
    std::unique_ptr<I_FileRWHandler> &&log_fh_counter)
    : plain_ni_fh(std::move(plain_ni_fh)),
      mapped_ni_fh(std::move(mapped_ni_fh)), log_fh(std::move(log_fh)),
      log_fh_counter(std::move(log_fh_counter)),
      nodes_sequence(
          std::make_unique<NodesSequence>(NodesSequence::from_input_stream(
              *this->plain_ni_fh->get_reader(std::ios::binary)))),
      nodes_map(NodesMapFactory::from_input_stream(
          *this->mapped_ni_fh->get_reader(std::ios::binary))),
      last_assigned_id(find_last_assigned()),
      log_writer(this->log_fh->get_writer(std::ios::binary | std::ios::app)),
      counter_writer(this->log_fh_counter->get_writer(std::ios::binary)) {
  this->nodes_map->restore(this->log_fh->get_reader(std::ios::binary),
                           this->log_fh_counter->get_reader(std::ios::binary));

  auto counter_reader = this->log_fh_counter->get_reader(std::ios::binary);
  auto &is = counter_reader->get_istream();
  is.seekg(0);
  logs_number = read_u64(is);
}

uint64_t NodeIdsManagerImpl::get_id(uint64_t real_id) {
  auto plain_id = nodes_sequence->get_id(real_id);
  if (plain_id != NOT_FOUND_NODEID)
    return plain_id;
  return nodes_map->get_id(real_id);
}
uint64_t NodeIdsManagerImpl::get_real_id(uint64_t mapped_id, int *err_code = nullptr) {
  int err_code2 = 0;
  auto from_seq = nodes_sequence->get_real_id(mapped_id, &err_code2);
  if (err_code2 == (int)NidsErrCode::SUCCESS_ERR_CODE){
    if(err_code != nullptr){
      *err_code = (int)NidsErrCode::SUCCESS_ERR_CODE;
    }
    return from_seq;
  }
  return nodes_map->get_real_id(mapped_id, err_code);
}
uint64_t NodeIdsManagerImpl::get_id_or_create(uint64_t real_id) {
  auto mapped_id = get_id(real_id);
  if (mapped_id != NOT_FOUND_NODEID)
    return mapped_id;
  last_assigned_id++;
  log_new_kv(real_id, last_assigned_id);
  nodes_map->add(real_id, last_assigned_id);
  return last_assigned_id;
}
uint64_t NodeIdsManagerImpl::find_last_assigned() {
  if (nodes_map->get_last_assigned() == NOT_FOUND_NODEID)
    return nodes_sequence->get_last_assigned();
  return nodes_map->get_last_assigned();
}
void NodeIdsManagerImpl::log_new_kv(uint64_t real_id, uint64_t mapped_id) {
  auto &os = log_writer->get_ostream();
  write_u64(os, real_id);
  write_u64(os, mapped_id);
  log_writer->flush();
  auto &os_c = counter_writer->get_ostream();
  os_c.seekp(0);
  write_u64(os_c, logs_number + 1);
  logs_number++;
}
NodesSequence &NodeIdsManagerImpl::get_nodes_sequence() {
  return *nodes_sequence;
}

} // namespace k2cache
