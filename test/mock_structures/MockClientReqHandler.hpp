//
// Created by cristobal on 12-10-22.
//

#ifndef RDFCACHEK2_MOCKCLIENTREQHANDLER_HPP
#define RDFCACHEK2_MOCKCLIENTREQHANDLER_HPP
#include "server/ClientReqHandler.hpp"
namespace  k2cache{
class MockClientReqHandler: public ClientReqHandler {
  std::unique_ptr<Message> next_msg;
  std::function<void(const std::string&)> response_handler;
public:
  explicit MockClientReqHandler(
      std::function<void(const std::string &)> response_handler);

  std::unique_ptr<Message> get_next_message() override;
  void send_response(const std::string &response) override;

  void set_next_message(std::unique_ptr<Message>&&msg);
};
}

#endif // RDFCACHEK2_MOCKCLIENTREQHANDLER_HPP
