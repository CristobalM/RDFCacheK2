//
// Created by cristobal on 27-06-22.
//

#include "ClientReqHandlerImpl.hpp"
#include "messages/utils.hpp"
#include <netinet/in.h>
namespace k2cache {

std::unique_ptr<Message> ClientReqHandlerImpl::get_next_message() {
  uint32_t msg_size = 0;

  bool was_read = read_nbytes_from_socket(
      client_socket_fd, reinterpret_cast<char *>(&msg_size), sizeof(msg_size));
  if (!was_read) {
    std::cerr << "Error while reading msg_size data from connection"
              << std::endl;
    return nullptr;
  }

  msg_size = ntohl(msg_size);
  auto message = std::make_unique<Message>(msg_size);

  was_read = read_nbytes_from_socket(client_socket_fd, message->get_buffer(),
                                     message->get_size());
  if (!was_read) {
    std::cerr << "Error while reading data from connection" << std::endl;
    return nullptr;
  }

  message->deserialize();
  return message;
}

ClientReqHandlerImpl::ClientReqHandlerImpl(int client_socket_fd)
    : client_socket_fd(client_socket_fd) {}
void ClientReqHandlerImpl::send_response(const std::string &response) {
  send(client_socket_fd, response.data(), response.size() * sizeof(char), 0);
}

} // namespace k2cache
