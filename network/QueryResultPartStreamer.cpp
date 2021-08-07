//
// Created by cristobal on 23-07-21.
//
#include "QueryResultPartStreamer.hpp"
#include "CacheServerTaskProcessor.hpp"
#include <map>
QueryResultPartStreamer::QueryResultPartStreamer(
    int id, std::shared_ptr<QueryResultIterator> query_result_iterator,
    std::unique_ptr<TimeControl> &&time_control, size_t threshold_part_size,
    std::shared_ptr<const std::vector<unsigned long>> predicates_in_use,
    TaskProcessor *task_processor)
    : id(id), query_result_iterator(std::move(query_result_iterator)),
      time_control(std::move(time_control)),
      predicates_in_use(std::move(predicates_in_use)),
      task_processor(task_processor), threshold_part_size(threshold_part_size),
      first(true), done(false) {}
proto_msg::CacheResponse QueryResultPartStreamer::get_next_response() {
  if (!time_control->tick())
    return time_control_finished_error();

  proto_msg::CacheResponse result;

  result.set_response_type(proto_msg::RESULT_TABLE_PART_RESPONSE);
  auto *proto_response = result.mutable_result_table_part_response();
  proto_response->set_id(id);
  auto *proto_rows = proto_response->mutable_rows();

  auto &result_it = query_result_iterator->get_it();

  size_t accumulated_size = 0;

  if (first) {
    auto &headers = result_it.get_headers();
    auto *proto_headers = proto_response->mutable_headers();
    auto &vim = query_result_iterator->get_vim();
    auto headers_map = vim.reverse();
    for (auto header_id : headers) {
      auto header_value = headers_map[header_id];
      accumulated_size += header_value.size();
      proto_headers->Add(std::move(header_value));
    }
    first = false;
  }
  if (!time_control->tick())
    return time_control_finished_error();

  while (result_it.has_next()) {
    auto next_row = result_it.next();
    if (!time_control->tick())
      return time_control_finished_error();

    auto *proto_row = proto_rows->Add();

    accumulated_size += next_row.size() * sizeof(unsigned long);
    for (auto resource_id : next_row) {
      auto *row_value = proto_row->mutable_row_values()->Add();
      auto resource = query_result_iterator->extract_resource(resource_id);
      accumulated_size += resource.value.size();
      auto proto_type = resource.get_proto_type();
      accumulated_size += sizeof(proto_type);
      row_value->set_value(std::move(resource.value));
      row_value->set_term_type(proto_type);
    }
    if (accumulated_size > threshold_part_size) {
      break;
    }
  }

  if (!result_it.has_next()) {
    proto_response->set_last_part(true);
    set_finished();
  } else {
    proto_response->set_last_part(false);
  }

  return result;
}

int QueryResultPartStreamer::get_id() { return id; }
proto_msg::CacheResponse QueryResultPartStreamer::timeout_proto() {
  set_finished();
  proto_msg::CacheResponse result;
  result.set_response_type(proto_msg::TIMED_OUT_RESPONSE);
  result.mutable_error_response();
  return result;
}

void QueryResultPartStreamer::set_finished() { done = true; }
bool QueryResultPartStreamer::all_sent() { return done; }
proto_msg::CacheResponse
QueryResultPartStreamer::time_control_finished_error() {
  if (!time_control->has_error())
    return timeout_proto();
  set_finished();
  std::cerr << "Query stopped early due to error: "
            << time_control->get_query_error().get_str() << std::endl;
  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(
      proto_msg::MessageType::INVALID_QUERY_RESPONSE);
  cache_response.mutable_invalid_query_response();
  return cache_response;
}
const std::vector<unsigned long> &
QueryResultPartStreamer::get_predicates_in_use() {
  return *predicates_in_use;
}
QueryResultPartStreamer::~QueryResultPartStreamer() {
  if (!predicates_in_use->empty())
    task_processor->mark_ready(*predicates_in_use);
}
