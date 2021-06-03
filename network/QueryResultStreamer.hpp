//
// Created by cristobal on 6/2/21.
//

#ifndef RDFCACHEK2_QUERYRESULTSTREAMER_HPP
#define RDFCACHEK2_QUERYRESULTSTREAMER_HPP

#include <PredicatesCacheManager.hpp>
#include <cstdint>
#include <query_processing/QueryResult.hpp>
#include <response_msg.pb.h>
#include <set>
class QueryResultStreamer {
  std::vector<uint64_t> keys;
  QueryResult query_result;
  int id;
  PredicatesCacheManager *cm;

  size_t keys_sent;
  size_t rows_sent;
  ResultTable::lvul_t::iterator rows_it;

public:
  QueryResultStreamer(std::set<uint64_t> &&keys, QueryResult &&query_result,
                      int id, PredicatesCacheManager *cm);

  QueryResultStreamer(const QueryResultStreamer &other) = delete;
  QueryResultStreamer &operator=(const QueryResultStreamer &other) = delete;

  QueryResultStreamer(QueryResultStreamer &&other) noexcept;
  QueryResultStreamer &operator=(QueryResultStreamer &&other) noexcept;

  proto_msg::CacheResponse get_next_response();
  bool keys_done();
  void
  populate_with_remaining_keys(proto_msg::ResultTablePartResponse &response);
  void populate_headers(proto_msg::KeysPart &part);
  void
  populate_with_remaining_rows(proto_msg::ResultTablePartResponse &response);
};

#endif // RDFCACHEK2_QUERYRESULTSTREAMER_HPP
