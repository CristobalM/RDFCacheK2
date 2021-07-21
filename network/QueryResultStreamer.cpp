//
// Created by cristobal on 6/2/21.
//

#include "QueryResultStreamer.hpp"
#include "network_msg_definitions.hpp"
QueryResultStreamer::QueryResultStreamer(
    std::set<uint64_t> &&keys, std::shared_ptr<QueryResult> query_result,
    int id, PredicatesCacheManager *cm)
    : keys(keys.begin(), keys.end()), query_result(std::move(query_result)),
      id(id), cm(cm), keys_sent(0), rows_sent(0),
      rows_it(this->query_result->table().get_data().begin()),
      first_key_part_sent(false) {}
proto_msg::CacheResponse QueryResultStreamer::get_next_response() {
  proto_msg::CacheResponse response;

  response.set_response_type(
      proto_msg::MessageType::RESULT_TABLE_PART_RESPONSE);
  auto *part_response = response.mutable_result_table_part_response();

  if (!first_key_part_sent || !keys_done()) {
    std::cout << id << " / populating with keys" << std::endl;
    populate_with_remaining_keys(*part_response);
    first_key_part_sent = true;
  } else {
    std::cout << id << "/ populating with rows" << std::endl;
    populate_with_remaining_rows(*part_response);
  }

  std::cout << id << "/ created response of type RESULT_TABLE_PART_RESPONSE"
            << std::endl;

  return response;
}
QueryResultStreamer::QueryResultStreamer(QueryResultStreamer &&other) noexcept
    : keys(std::move(other.keys)), query_result(std::move(other.query_result)),
      id(other.id), cm(other.cm), keys_sent(other.keys_sent),
      rows_sent(other.rows_sent), rows_it(other.rows_it) {}

QueryResultStreamer &
QueryResultStreamer::operator=(QueryResultStreamer &&other) noexcept {
  keys = std::move(other.keys);
  query_result = std::move(other.query_result);
  id = other.id;
  cm = other.cm;
  keys_sent = other.keys_sent;
  rows_sent = other.rows_sent;
  rows_it = other.rows_it;
  return *this;
}
bool QueryResultStreamer::keys_done() {
  std::cout << "keys sent: " << keys_sent << "/" << keys.size() << std::endl;
  return static_cast<unsigned long>(keys_sent) == keys.size();
}
void QueryResultStreamer::populate_with_remaining_keys(
    proto_msg::ResultTablePartResponse &response) {

  auto *keys_part = response.mutable_keys_part();

  if (keys_sent == 0)
    populate_headers(*keys_part);

  auto last_id = cm->get_last_id();
  size_t i = keys_sent;

  for (; i < keys.size(); i++) {
    RDFResource key_res;
    auto key = keys[i];

    if (key > last_id) {
      key_res = query_result->get_extra_dict().extract_resource(key - last_id);
    } else {
      key_res = cm->extract_resource(key);
    }

    auto *kv = keys_part->add_kvs();
    kv->set_key(key);
    kv->set_value(key_res.value);
    if (response.ByteSizeLong() > MAX_PROTO_MESSAGE_SIZE_ALLOWED) {
      break;
    }
  }

  keys_sent = i;
  bool last_part = keys_sent == keys.size();
  std::cout << "keys last part: " << last_part << std::endl;
  keys_part->set_last_part(last_part);
}
void QueryResultStreamer::populate_headers(proto_msg::KeysPart &part) {
  auto reversed_indexes = query_result->get_vim().reverse();
  for (auto header : query_result->table().headers) {
    const auto &var = reversed_indexes[header];
    part.add_header(var);
  }
}
void QueryResultStreamer::populate_with_remaining_rows(
    proto_msg::ResultTablePartResponse &response) {

  auto *rows_part = response.mutable_rows_part();

  std::cout << "starting from row: " << rows_sent << " out of "
            << query_result->table().get_data().size() << std::endl;

  for (; rows_it != query_result->table().get_data().end(); rows_it++) {
    auto *row_proto = rows_part->mutable_rows()->Add();
    const auto &row = *rows_it;
    for (auto value : row) {
      row_proto->add_row(value);
    }
    rows_sent++;
    if (response.ByteSizeLong() > MAX_PROTO_MESSAGE_SIZE_ALLOWED) {
      break;
    }
  }
  auto last_part = rows_sent == query_result->table().get_data().size();
  std::cout << "last_part: " << last_part << std::endl;
  rows_part->set_last_part(last_part);
}
bool QueryResultStreamer::all_sent() {
  return rows_it == this->query_result->table().get_data().end();
}
