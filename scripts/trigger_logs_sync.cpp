//
// Created by cristobal on 17-05-22.
//

#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>

#include <getopt.h>

#include "hashing.hpp"
#include "messages/utils.hpp"
#include "serialization_util.hpp"
#include <message_type.pb.h>
#include <request_msg.pb.h>
#include <response_msg.pb.h>
#include "custom_endian.hpp"

using namespace k2cache;

struct parsed_options {
  std::string hostname;
  int port;
};

void send_request(int socket_fd, const proto_msg::CacheRequest &request);

parsed_options read_server_options(int argc, char **argv);

int connect_to_server(const parsed_options &options);
int main(int argc, char **argv) {
  proto_msg::CacheRequest cache_request;
  cache_request.set_request_type(proto_msg::SYNC_LOGS_WITH_INDEXES_REQUEST);
  auto server_options = read_server_options(argc, argv);
  int socket_fd = connect_to_server(server_options);
  if (socket_fd < 0) {
    return 1;
  }
  std::cout << "sending sync request" << std::endl;
  send_request(socket_fd, cache_request);

  proto_msg::CacheRequest end_connection_request;
  end_connection_request.set_request_type(proto_msg::CONNECTION_END);
  std::cout << "\n\n\nsending end connection request" << std::endl;
  send_request(socket_fd, end_connection_request);
}
int connect_to_server(const parsed_options &options) {
  int sock_fd;
  if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std::cerr << "Socket creation error" << std::endl;
    return -1;
  }
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(options.port);

  if (inet_pton(AF_INET, options.hostname.data(), &serv_addr.sin_addr) <= 0) {
    std::cerr << "Invalid address " << options.hostname << std::endl;
    return -1;
  }

  if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    std::cerr << "Connection to " << options.hostname << " failed" << std::endl;
    return -1;
  }
  return sock_fd;
}

parsed_options read_server_options(int argc, char **argv) {
  const char short_options[] = "h:p:";
  struct option long_options[] = {
      {"hostname", required_argument, nullptr, 'h'},
      {"port", required_argument, nullptr, 'p'},
  };

  int opt, opt_index;

  bool has_host = false;
  bool has_port = false;
  parsed_options out{};

  while ((
      opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
    if (opt == -1) {
      break;
    }
    switch (opt) {
    case 'h':
      out.hostname = optarg;
      has_host = true;
      break;
    case 'p':
      out.port = std::stoi(optarg);
      has_port = true;
      break;
    default:
      break;
    }
  }

  if (!has_host)
    throw std::runtime_error("hostname (h) argument is required");
  if (!has_port)
    throw std::runtime_error("port (p) argument is required");
  return out;
}

void send_request(int socket_fd, const proto_msg::CacheRequest &request) {
  std::string serialized = request.SerializeAsString();
  uint32_t message_size = serialized.size();
  std::stringstream ss;
  write_u32(ss, message_size);
  ss.write(serialized.data(), sizeof(char) * serialized.size());
  auto result = ss.str();
  int bytes_sent =
      send(socket_fd, result.data(), result.size() * sizeof(char), 0);
  if (bytes_sent < 0) {
    std::cerr << "failed to send request, was_read code: " << bytes_sent
              << std::endl;
    return;
  }

  // Getting Message Size
  uint64_t msg_sz;
  std::cout << "reading first " << sizeof(msg_sz) << " bytes" << std::endl;
  bool was_read = read_nbytes_from_socket(
      socket_fd, reinterpret_cast<char *>(&msg_sz), sizeof(msg_sz));
  if (!was_read) {
    std::cerr << "Error while reading msg_size data from connection"
              << std::endl;
    return;
  }
  msg_sz = be64toh(msg_sz);

  if (msg_sz > 65000 || msg_sz <= 0) {
    std::cerr << "invalid message size: " << msg_sz << std::endl;
    return;
  }

  std::array<char, 16> md5_received_hash{};

  // Getting md5 hash
  was_read = read_nbytes_from_socket(socket_fd, md5_received_hash.data(),
                                     md5_received_hash.size());
  if (!was_read) {
    std::cerr << "error, failed to read md5 hash message" << std::endl;
    return;
  }

  // Getting message
  std::vector<char> buf(msg_sz);
  was_read = read_nbytes_from_socket(socket_fd, buf.data(), msg_sz);
  if (!was_read) {
    std::cerr << "error, failed to read message" << std::endl;
    return;
  }

  proto_msg::CacheResponse cache_response;
  std::istringstream iss(buf.data(), std::ios::in | std::ios::binary);

  auto md5sum_result = md5calc(buf);
  if (md5sum_result != md5_received_hash) {
    std::cerr << "hashes don't match, received: '"
              << md5_human_readable(md5_received_hash) << "'"
              << " message sum: '" << md5_human_readable(md5sum_result) << "'"
              << std::endl;
  }

  auto deserialized = std::make_unique<proto_msg::CacheResponse>();
  deserialized->ParseFromArray(buf.data(), (int)msg_sz);
  std::cout << "Received: "
            << proto_msg::MessageType_Name(deserialized->response_type())
            << " response type code: " << deserialized->response_type()
            << "\nmessage size: " << msg_sz << std::endl;
}
