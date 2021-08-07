//
// Created by cristobal on 6/2/21.
//

#ifndef RDFCACHEK2_TASKPROCESSOR_HPP
#define RDFCACHEK2_TASKPROCESSOR_HPP

#include "I_QRStreamer.hpp"
#include "TimeControl.hpp"
#include <cstdint>
#include <query_processing/QueryResultIterator.hpp>
#include <set>
#include <utility>

class TaskProcessor {
public:
  virtual I_QRStreamer &get_streamer(int id) = 0;
  virtual ~TaskProcessor() = default;
  virtual bool has_streamer(int id) = 0;
  virtual I_QRStreamer &create_streamer(
      std::shared_ptr<QueryResultIterator> query_result_iterator,
      std::unique_ptr<TimeControl> &&time_control,
      std::shared_ptr<const std::vector<unsigned long>> predicates_in_use) = 0;
  virtual void clean_streamer(int id) = 0;
  virtual void process_missed_predicates(
      std::shared_ptr<const std::vector<unsigned long>> predicates) = 0;
  virtual void mark_using(const std::vector<unsigned long> &predicates) = 0;
  virtual void
  mark_ready(const std::vector<unsigned long> &predicates_in_use) = 0;
  virtual std::mutex &get_replacement_mutex() = 0;
};

#endif // RDFCACHEK2_TASKPROCESSOR_HPP
