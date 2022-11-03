//
// Created by cristobal on 12-10-22.
//

#ifndef RDFCACHEK2_MOCKCLIENTREQHANDLER_HPP
#define RDFCACHEK2_MOCKCLIENTREQHANDLER_HPP
#include "server/ClientReqHandler.hpp"
namespace  k2cache{
class MockClientReqHandler: public ClientReqHandler {
  std::unique_ptr<Message> next_msg;
  std::string last_message;
public:

  std::unique_ptr<Message> get_next_message() override;
  void send_response(const std::string &response) override;

  void set_next_req_msg(std::unique_ptr<Message>&&msg);

  const std::string &get_last_resp_msg();
};
}

#endif // RDFCACHEK2_MOCKCLIENTREQHANDLER_HPP
