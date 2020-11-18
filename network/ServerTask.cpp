//
// Created by Cristobal Miranda, 2020
//

#include <iostream>
#include <sstream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>


#include "ServerTask.hpp"

#include <graph_result.pb.h>
#include <message_type.pb.h>
#include <response_msg.pb.h>

ServerTask::ServerTask(int client_socket_fd, Cache &cache)
    : client_socket_fd(client_socket_fd), cache(cache) {}

void send_response(int socket_client_fd,
                   proto_msg::CacheResponse &cache_response) {
  std::string serialized = cache_response.SerializeAsString();
  uint32_t message_sz = htonl(serialized.size() + sizeof(uint32_t));
  std::stringstream ss;
  ss.write(reinterpret_cast<char *>(&message_sz), sizeof(uint32_t));
  ss.write(serialized.data(), sizeof(char) * serialized.size());
  auto result = ss.str();
  send(socket_client_fd, result.data(), result.size() * sizeof(char), 0);
}




proto_msg::CacheResponse create_response_from_query_result(QueryResult &query_result){

}

void process_cache_query(ServerTask &server_task, Message &message){
  auto &cache = server_task.get_cache();
  auto &tree = message.get_cache_request().cache_run_query_algebra().sparql_tree();
  auto query_result = cache.run_query(tree);
  auto response = create_response_from_query_result(query_result);
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

    msg_size -= sizeof(msg_size);

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
