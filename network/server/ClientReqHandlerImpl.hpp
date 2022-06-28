//
// Created by cristobal on 27-06-22.
//

#ifndef RDFCACHEK2_CLIENTREQHANDLERIMPL_HPP
#define RDFCACHEK2_CLIENTREQHANDLERIMPL_HPP
#include "ClientReqHandler.hpp"
namespace k2cache {
class ClientReqHandlerImpl : public ClientReqHandler {
  int client_socket_fd;
public:
  explicit ClientReqHandlerImpl(int client_socket_fd);
  std::unique_ptr<Message> get_next_message() override;
  void send_response(const std::string &response) override;
};
} // namespace k2cache

#endif // RDFCACHEK2_CLIENTREQHANDLERIMPL_HPP
