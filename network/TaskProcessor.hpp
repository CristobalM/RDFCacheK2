//
// Created by cristobal on 6/2/21.
//

#ifndef RDFCACHEK2_TASKPROCESSOR_HPP
#define RDFCACHEK2_TASKPROCESSOR_HPP

#include "I_TRStreamer.hpp"
#include "I_Updater.hpp"
#include "K2TreeUpdates.hpp"
#include "WriteDataLock.hpp"
#include <cstdint>
#include <set>
#include <utility>

class TaskProcessor {
public:
  virtual ~TaskProcessor() = default;

  virtual I_TRStreamer &get_triple_streamer(int id) = 0;
  virtual bool has_triple_streamer(int channel_id) = 0;
  virtual void clean_triple_streamer(int id) = 0;
  virtual void process_missed_predicates(
      std::shared_ptr<const std::vector<unsigned long>> predicates) = 0;
  virtual void mark_using(const std::vector<unsigned long> &predicates) = 0;
  virtual void
  mark_ready(const std::vector<unsigned long> &predicates_in_use) = 0;
  virtual I_TRStreamer &
  create_triples_streamer(std::vector<unsigned long> &&loaded_predicates) = 0;
  virtual int begin_update_session() = 0;
  virtual I_Updater &get_updater(int updater_id) = 0;
  virtual void log_updates(std::vector<K2TreeUpdates> &k2trees_updates) = 0;
  virtual WriteDataLock acquire_write_lock() = 0;
  virtual void sync_logs_with_indexes() = 0;
};

#endif // RDFCACHEK2_TASKPROCESSOR_HPP
