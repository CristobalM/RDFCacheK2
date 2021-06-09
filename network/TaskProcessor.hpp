//
// Created by cristobal on 6/2/21.
//

#ifndef RDFCACHEK2_TASKPROCESSOR_HPP
#define RDFCACHEK2_TASKPROCESSOR_HPP

#include "QueryResultStreamer.hpp"
#include <cstdint>
#include <query_processing/QueryResult.hpp>
#include <set>
#include <utility>

class TaskProcessor {
public:
  virtual QueryResultStreamer &get_streamer(int id) = 0;
  virtual QueryResultStreamer &create_streamer(std::set<uint64_t> &&keys,
                                               QueryResult &&query_result) = 0;
  virtual ~TaskProcessor() = default;
  virtual bool has_streamer(int id) = 0;
};

#endif // RDFCACHEK2_TASKPROCESSOR_HPP
