//
// Created by cristobal on 23-07-21.
//

#ifndef RDFCACHEK2_QUERYRESULTPARTSTREAMER_HPP
#define RDFCACHEK2_QUERYRESULTPARTSTREAMER_HPP

#include "CacheServerTaskProcessor.hpp"
#include "I_QRStreamer.hpp"
#include <TimeControl.hpp>
#include <memory>
#include <query_processing/QueryResultIteratorHolder.hpp>
class QueryResultPartStreamer : public I_QRStreamer {
  const int id;
  std::shared_ptr<QueryResultIteratorHolder> query_result_iterator;
  std::unique_ptr<TimeControl> time_control;
  std::shared_ptr<const std::vector<unsigned long>> predicates_in_use;

  TaskProcessor *task_processor;

  const size_t threshold_part_size;
  bool first;
  bool done;

public:
  QueryResultPartStreamer(
      int id, std::shared_ptr<QueryResultIteratorHolder> query_result_iterator,
      std::unique_ptr<TimeControl> &&time_control, size_t threshold_part_size,
      std::shared_ptr<const std::vector<unsigned long>> predicates_in_use,
      TaskProcessor *task_processor);

  ~QueryResultPartStreamer() override;
  proto_msg::CacheResponse get_next_response() override;
  int get_id() override;
  proto_msg::CacheResponse timeout_proto();
  void set_finished();
  bool all_sent() override;
  proto_msg::CacheResponse time_control_finished_error();
  const std::vector<unsigned long> &get_predicates_in_use() override;
};

#endif // RDFCACHEK2_QUERYRESULTPARTSTREAMER_HPP
