//
// Created by cristobal on 6/2/21.
//

#ifndef RDFCACHEK2_TASKPROCESSOR_HPP
#define RDFCACHEK2_TASKPROCESSOR_HPP

#include "BGPMessage.hpp"
#include "WriteDataLock.hpp"
#include "server/session/Updater.hpp"
#include "streaming/I_BGPStreamer.hpp"
#include "streaming/I_TRStreamer.hpp"
#include "updating/K2TreeUpdates.hpp"
#include <cstdint>
#include <set>
#include <utility>

namespace k2cache {
class TaskProcessor {
public:
  virtual ~TaskProcessor() = default;

  virtual I_TRStreamer &get_triple_streamer(int id) = 0;
  virtual bool has_triple_streamer(int channel_id) = 0;
  virtual void clean_triple_streamer(int id) = 0;
  virtual void process_missed_predicates(
      std::shared_ptr<const std::vector<uint64_t>> predicates) = 0;
  virtual void mark_using(const std::vector<uint64_t> &predicates) = 0;
  virtual void
  mark_ready(const std::vector<uint64_t> &predicates_in_use) = 0;
  virtual I_TRStreamer &
  create_triples_streamer(std::vector<uint64_t> &&loaded_predicates) = 0;
  virtual int begin_update_session() = 0;
  virtual Updater &get_updater(int updater_id) = 0;
  virtual void log_updates(std::vector<K2TreeUpdates> &k2trees_updates) = 0;
  virtual WriteDataLock acquire_write_lock() = 0;
  virtual void sync_to_persistent() = 0;
  virtual I_BGPStreamer &get_bgp_streamer(BGPMessage message) = 0;
  virtual I_BGPStreamer * get_existing_bgp_streamer(int channel_id) = 0;
  virtual void clean_bgp_streamer(int channel_id) = 0;
};
} // namespace k2cache

#endif // RDFCACHEK2_TASKPROCESSOR_HPP
