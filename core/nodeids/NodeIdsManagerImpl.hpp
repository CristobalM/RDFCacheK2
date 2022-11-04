//
// Created by cristobal on 26-06-22.
//

#ifndef RDFCACHEK2_NODEIDSMANAGERIMPL_HPP
#define RDFCACHEK2_NODEIDSMANAGERIMPL_HPP

#include "I_FileRWHandler.hpp"
#include "NodeIdsManager.hpp"
#include "NodesMap.hpp"
#include "NodesSequence.hpp"
#include <memory>

namespace k2cache {
class NodeIdsManagerImpl : public NodeIdsManager {
  std::unique_ptr<I_FileRWHandler> plain_ni_fh;
  std::unique_ptr<I_FileRWHandler> mapped_ni_fh;
  std::unique_ptr<I_FileRWHandler> log_fh;
  std::unique_ptr<I_FileRWHandler> log_fh_counter;

  std::unique_ptr<NodesSequence> nodes_sequence;
  std::unique_ptr<NodesMap> nodes_map;

  long last_assigned_id;

  std::unique_ptr<I_OStream> log_writer;
  std::unique_ptr<I_OStream> counter_writer;

  long logs_number;

public:
  NodeIdsManagerImpl(std::unique_ptr<I_FileRWHandler> &&plain_ni_fh,
                     std::unique_ptr<I_FileRWHandler> &&mapped_ni_fh,
                     std::unique_ptr<I_FileRWHandler> &&log_fh,
                     std::unique_ptr<I_FileRWHandler> &&log_fh_counter);

  long get_id(long real_id) override;
  long get_real_id(long mapped_id) override;
  long get_id_or_create(long real_id) override;
  long find_last_assigned();
  void log_new_kv(long real_id, long mapped_id);
};
} // namespace k2cache

#endif // RDFCACHEK2_NODEIDSMANAGERIMPL_HPP
