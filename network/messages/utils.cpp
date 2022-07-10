//
// Created by cristobal on 18-05-22.
//

#include "utils.hpp"
#include <cstdio>
#include <iostream>
#include <sys/socket.h>
#include <thread>
namespace k2cache {
bool read_nbytes_from_socket(int client_socket_fd, char *read_buffer,
                             size_t bytes_to_read) {
  std::size_t offset = 0;
  for (;;) {
    ssize_t bytes_read = recv(client_socket_fd, read_buffer + offset,
                              bytes_to_read - offset, MSG_WAITALL);
    if (bytes_read < 0) {
      if (errno != EINTR) {
        std::cerr << "IO Error while reading from socket" << std::endl;
        return false;
      }
    } else if (bytes_read == 0) {
      if (offset == 0) {
        std::cerr << "Unexpected end of stream with offset 0" << std::endl;
        return false;
      } else {
        std::cerr << "Unexpected end of stream" << std::endl;
        return false;
      }
    } else if (offset + bytes_read == bytes_to_read) {
      return true;
    } else {
      offset += bytes_read;
    }
  }
}
TripleNodeId
proto_triple_to_internal(const proto_msg::TripleNodeIdEnc &proto_triple) {
  return TripleNodeId(NodeId((long)proto_triple.subject().encoded_data()),
                      NodeId((long)proto_triple.predicate().encoded_data()),
                      NodeId((long)proto_triple.object().encoded_data()));
}
} // namespace k2cache
