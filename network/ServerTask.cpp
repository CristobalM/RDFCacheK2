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

#include <graph_result.pb.h>
#include <message_type.pb.h>
#include <response_msg.pb.h>

#include <hashing.hpp>
#include <serialization_util.hpp>

ServerTask::ServerTask(int client_socket_fd, Cache &cache)
    : client_socket_fd(client_socket_fd), cache(cache) {}

void send_response(int socket_client_fd,
                   proto_msg::CacheResponse &cache_response) {
  // std::cout << "Sending response: " << cache_response.DebugString() <<
  // std::endl;
  std::string serialized = cache_response.SerializeAsString();
  auto serialized_hash = md5calc(serialized);
  std::stringstream ss;
  std::cout << "Sending message of size " << serialized.size() << " with hash '"
            << md5_human_readable(serialized_hash) << "'" << std::endl;
  write_u64(ss, serialized.size());
  ss.write(serialized_hash.data(), sizeof(char) * serialized_hash.size());
  ss.write(serialized.data(), sizeof(char) * serialized.size());
  // ss << serialized_hash;
  // ss << serialized;
  auto result = ss.str();
  send(socket_client_fd, result.data(), result.size() * sizeof(char), 0);
}

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

static proto_msg::CacheResponse
create_response_from_query_result(ServerTask &server_task,
                                  QueryResult &query_result, Message &message) {
  auto &vim = query_result.get_vim();
  auto &table = query_result.table();

  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(
      proto_msg::MessageType::RESULT_TABLE_RESPONSE);

  auto &tree =
      message.get_cache_request().cache_run_query_algebra().sparql_tree();

  auto &project_node = tree.root().project_node();

  auto &headers = query_result.table().headers;
  std::unordered_map<unsigned long, unsigned long> reversed_uindexes;
  for (size_t i = 0; i < headers.size(); i++) {
    reversed_uindexes[headers[i]] = i;
  }

  std::vector<unsigned long> resulting_vars_order;

  for (int i = 0; i < project_node.vars_size(); i++) {
    const auto &var = project_node.vars(i);
    auto var_index = vim.var_indexes[var];
    resulting_vars_order.push_back(reversed_uindexes[var_index]);
  }

  std::set<uint64_t> keys; // will store values in ascending order
  for (const auto &row : table.get_data()) {
    auto *response_row =
        cache_response.mutable_query_result_response()->add_rows();
    for (size_t i = 0; i < row.size(); i++) {
      auto value = row[resulting_vars_order[i]];
      keys.insert(value);
      response_row->add_row(value);
    }
  }

  auto &cache = server_task.get_cache();

  for (auto key : keys) {
    auto key_res = cache.extract_resource(key);
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
      kv->set_type(proto_msg::TermType::LITERAL);
      break;
    }
  }

  for (int i = 0; i < project_node.vars_size(); i++) {
    const auto &var = project_node.vars(i);
    cache_response.mutable_query_result_response()->add_header(var);
  }

  return cache_response;
}

void process_cache_query(ServerTask &server_task, Message &message) {
  auto &cache = server_task.get_cache();
  auto &tree =
      message.get_cache_request().cache_run_query_algebra().sparql_tree();
  auto query_result = cache.run_query(tree);
  auto response =
      create_response_from_query_result(server_task, query_result, message);
  int client_fd = server_task.get_client_socket_fd();
  send_response(client_fd, response);
}

void process_connection_end(ServerTask &server_task, Message &) {
  int client_fd = server_task.get_client_socket_fd();

  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(proto_msg::MessageType::CONNECTION_END);
  cache_response.mutable_connection_end_response()->set_end(true);

  send_response(client_fd, cache_response);
}

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
      process_cache_query(*this, message);
      break;
    case proto_msg::MessageType::CONNECTION_END:
      std::cout << "Request of type CONNECTION_END" << std::endl;
      process_connection_end(*this, message);
      return;
    default:
      break;
    }
  }
}

int ServerTask::get_client_socket_fd() { return client_socket_fd; }

Cache &ServerTask::get_cache() { return cache; }
