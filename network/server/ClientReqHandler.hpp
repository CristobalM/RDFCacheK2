//
// Created by cristobal on 27-06-22.
//

#ifndef RDFCACHEK2_CLIENTREQHANDLER_HPP
#define RDFCACHEK2_CLIENTREQHANDLER_HPP
#include "messages/Message.hpp"
#include <memory>
namespace k2cache {
struct ClientReqHandler {
  virtual ~ClientReqHandler() = default;
  virtual std::unique_ptr<Message> get_next_message() = 0;
  virtual void send_response(const std::string &response) = 0;
};
} // namespace k2cache
#endif // RDFCACHEK2_CLIENTREQHANDLER_HPP
