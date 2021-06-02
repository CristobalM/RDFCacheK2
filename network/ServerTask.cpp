//
// Created by Cristobal Miranda, 2020
//

#include <iostream>
#include <netinet/in.h>
#include <set>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>

#include "ServerTask.hpp"
#include "network_msg_definitions.hpp"

#include <graph_result.pb.h>
#include <message_type.pb.h>
#include <response_msg.pb.h>

#include <hashing.hpp>
#include <serialization_util.hpp>


ServerTask::ServerTask(int client_socket_fd, Cache &cache,
                       TaskProcessor &task_processor)
    : client_socket_fd(client_socket_fd), cache(cache),
      task_processor(task_processor) {}

// static void print_table_debug(QueryResult &query_result, ServerTask
// &server_task){
//   std::cout << " Debug print table" << std::endl;
//   auto &vim = query_result.get_vim();
//   auto &table = query_result.table();

//   if(table.headers.size() == 0) throw std::runtime_error("No headers present
//   in table");

//   auto rev_map = vim.reverse();
//   for(auto header: table.headers){
//     std::cout << rev_map[header] << ",";
//   }
//   std::cout << std::endl;

//   int counter = 0;
//   for(auto &row: table.data){
//     if(row.size() != table.headers.size()){
//       throw std::runtime_error(std::to_string(counter) + "th row has size " +
//       std::to_string(row.size()) + " but should be " +
//       std::to_string(table.headers.size()));
//     }
//     counter++;
//     for(auto col: row){
//       std::cout << server_task.get_cache().extract_resource(col).value <<
//       ",";
//     }
//     std::cout << "\n";
//   }
//   std::cout << std::endl;
// }

bool read_nbytes_from_socket(int client_socket_fd, char *read_buffer,
                             size_t bytes_to_read) {
  std::cout << "read_nbytes_from_socket: bytes_to_read:  " << bytes_to_read
            << std::endl;
  std::size_t offset = 0;
  for (;;) {
    ssize_t bytes_read = recv(client_socket_fd, read_buffer + offset,
                              bytes_to_read - offset, MSG_WAITALL);
    std::cout << "bytes read: " << bytes_read << std::endl;
    if (bytes_read < 0) {
      if (errno != EINTR) {
        std::cerr << "IO Error while reading from socket" << std::endl;
        return false;
      }
    } else if (bytes_read == 0) {
      if (offset == 0) {
        std::cout << "wtf" << std::endl;
        return false;
      } else {
        std::cerr << "Unexpected end of stream" << std::endl;
        return false;
      }
    } else if (offset + bytes_read == bytes_to_read) {
      std::cout << "finish reading " << bytes_to_read << "bytes" << std::endl;
      return true;
    } else {
      offset += bytes_read;
    }
  }
}

void ServerTask::process() {

  for (;;) {
    uint32_t msg_size = 0;

    bool was_read = read_nbytes_from_socket(client_socket_fd,
                                            reinterpret_cast<char *>(&msg_size),
                                            sizeof(msg_size));

    if (!was_read) {
      std::cerr << "Error while reading msg_size data from connection"
                << std::endl;
      return;
    }

    std::cout << "msg_size before: " << msg_size << std::endl;

    msg_size = ntohl(msg_size);
    if (msg_size <= sizeof(msg_size)) {
      std::cout << "Message with size " << msg_size << std::endl;
      return;
    }
    std::cout << "msg_size before rsz: " << msg_size << std::endl;

    std::cout << "msg_size after: " << msg_size << std::endl;

    std::cout << "Allocating message" << std::endl;
    Message message(msg_size);
    std::cout << "Message allocated" << std::endl;

    was_read = read_nbytes_from_socket(client_socket_fd, message.get_buffer(),
                                       message.get_size());
    if (!was_read) {
      std::cerr << "Error while reading data from connection" << std::endl;
      return;
    }

    message.deserialize();

    std::cout << "Incoming message" << message.get_buffer() << std::endl;

    switch (message.request_type()) {
    case proto_msg::MessageType::UNKNOWN:
      break;
    case proto_msg::MessageType::RUN_QUERY:
      std::cout << "Request of type RUN_QUERY" << std::endl;
      process_cache_query(message);
      break;
    case proto_msg::MessageType::CONNECTION_END:
      std::cout << "Request of type CONNECTION_END" << std::endl;
      process_connection_end();
      return;
    default:
      break;
    }
  }
}

int ServerTask::get_client_socket_fd() { return client_socket_fd; }

Cache &ServerTask::get_cache() { return cache; }

void ServerTask::process_cache_query(Message &message) {
  // auto &cache = get_cache();
  auto &tree =
      message.get_cache_request().cache_run_query_algebra().sparql_tree();
  auto valid = cache.query_is_valid(tree);

  if (!valid) {
    send_invalid_response();
    return;
  }
  auto query_result = cache.run_query(tree);

  auto response = create_response_from_query_result(std::move(query_result), message);
  send_response(response);
}

void ServerTask::send_invalid_response() {
  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(
      proto_msg::MessageType::INVALID_QUERY_RESPONSE);
  cache_response.mutable_invalid_query_response();
  send_response(cache_response);
}

proto_msg::CacheResponse
ServerTask::create_response_from_query_result(QueryResult &&query_result,
                                              Message &message) {
  auto &vim = query_result.get_vim();
  auto &table = query_result.table();

  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(
      proto_msg::MessageType::RESULT_TABLE_RESPONSE);

  auto &tree =
      message.get_cache_request().cache_run_query_algebra().sparql_tree();

  std::set<uint64_t> keys; // will store values in ascending order

  for (const auto &row : table.get_data()) {
    for (auto value : row) {
      keys.insert(value);
    }
  }

  size_t keys_size = 0;
  for(const auto &key: keys){
    RDFResource key_res;
    if (key > cache.get_pcm().get_last_id()) {
      key_res = query_result.get_extra_dict().extract_resource(
          key - cache.get_pcm().get_last_id());
    } else {
      key_res = cache.extract_resource(key);
    }
    keys_size += key_res.value.size() * sizeof(char);
  }
  keys_size += keys.size() * sizeof(uint64_t);
  keys_size += query_result.table().get_data().size() * query_result.table().headers.size() * sizeof(uint64_t);
  if(keys_size  > MAX_PROTO_MESSAGE_SIZE_ALLOWED){
    return create_parts_response(std::move(keys), std::move(query_result));
  }

  if (tree.root().node_case() == proto_msg::SparqlNode::kProjectNode) {
    auto &project_node = tree.root().project_node();
    auto &headers = query_result.table().headers;
    std::unordered_map<unsigned long, unsigned long> reversed_uindexes;
    for (size_t i = 0; i < headers.size(); i++) {
      reversed_uindexes[headers[i]] = i;
    }

    std::vector<unsigned long> resulting_vars_order;
    // auto resulting_vars_order = QueryVarsOrderExtractor(query_result,
    // tree.root()).extract();

    for (int i = 0; i < project_node.vars_size(); i++) {
      const auto &var = project_node.vars(i);
      auto var_index = vim.var_indexes[var];
      resulting_vars_order.push_back(reversed_uindexes[var_index]);
    }

    for (const auto &row : table.get_data()) {
      auto *response_row =
          cache_response.mutable_query_result_response()->add_rows();
      for (size_t i = 0; i < row.size(); i++) {
        auto value = row[resulting_vars_order[i]];
        response_row->add_row(value);
      }
    }
    for (int i = 0; i < project_node.vars_size(); i++) {
      const auto &var = project_node.vars(i);
      cache_response.mutable_query_result_response()->add_header(var);
    }
  } else {
    for (const auto &row : table.get_data()) {
      auto *response_row =
          cache_response.mutable_query_result_response()->add_rows();

      for (auto value : row) {
        response_row->add_row(value);
      }
    }
    auto reversed_indexes = vim.reverse();
    for (auto header : table.headers) {
      const auto &var = reversed_indexes[header];
      cache_response.mutable_query_result_response()->add_header(var);
    }
  }

  for (auto key : keys) {
    RDFResource key_res;
    if (key > cache.get_pcm().get_last_id()) {
      key_res = query_result.get_extra_dict().extract_resource(
          key - cache.get_pcm().get_last_id());
    } else {
      key_res = cache.extract_resource(key);
    }

    auto *kv = cache_response.mutable_query_result_response()->add_kvs();
    kv->set_key(key);
    kv->set_value(key_res.value);
    switch (key_res.resource_type) {
    case RDFResourceType::RDF_TYPE_BLANK:
      kv->set_type(proto_msg::TermType::BLANK_NODE);
      break;

    case RDFResourceType::RDF_TYPE_IRI:
      kv->set_type(proto_msg::TermType::IRI);
      break;

    case RDFResourceType::RDF_TYPE_LITERAL:
    case NULL_RESOURCE_TYPE:
      kv->set_type(proto_msg::TermType::LITERAL);
      break;
    }
  }

  return cache_response;
}
void ServerTask::send_response(proto_msg::CacheResponse &cache_response) {

  std::string serialized = cache_response.SerializeAsString();
  auto serialized_hash = md5calc(serialized);
  std::stringstream ss;
  std::cout << "Sending message of size " << serialized.size() << " with hash '"
            << md5_human_readable(serialized_hash) << "'" << std::endl;
  write_u64(ss, serialized.size());
  ss.write(serialized_hash.data(), sizeof(char) * serialized_hash.size());
  ss.write(serialized.data(), sizeof(char) * serialized.size());

  auto result = ss.str();
  send(client_socket_fd, result.data(), result.size() * sizeof(char), 0);
}
void ServerTask::process_connection_end() {
  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(proto_msg::MessageType::CONNECTION_END);
  cache_response.mutable_connection_end_response()->set_end(true);
  send_response(cache_response);
}
proto_msg::CacheResponse
ServerTask::create_parts_response(std::set<uint64_t> &&keys,
                                  QueryResult &&result) {
  auto streamer_data = task_processor.create_streamer(std::move(keys), std::move(result));
  auto &streamer = streamer_data.first;
  return streamer.get_next_response();
}
