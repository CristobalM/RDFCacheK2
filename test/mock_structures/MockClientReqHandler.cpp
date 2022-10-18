//
// Created by cristobal on 12-10-22.
//

#include "MockClientReqHandler.hpp"

#include <utility>

namespace k2cache {

std::unique_ptr<Message> MockClientReqHandler::get_next_message() {
  return std::make_unique<Message>(*next_msg);
}
void MockClientReqHandler::send_response(const std::string &s) {
  last_message = s;
}
void MockClientReqHandler::set_next_req_msg(std::unique_ptr<Message> &&msg) {
  next_msg = std::move(msg);
}
const std::string &MockClientReqHandler::get_last_resp_msg() {
  return last_message;
}
} // namespace k2cache