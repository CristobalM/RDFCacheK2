//
// Created by cristobal on 12-10-22.
//

#include "MockClientReqHandler.hpp"

#include <utility>

namespace k2cache {

std::unique_ptr<Message> MockClientReqHandler::get_next_message() {
  return std::make_unique<Message>(*next_msg);
}
void MockClientReqHandler::send_response(const std::string &) {}
void MockClientReqHandler::set_next_message(std::unique_ptr<Message> &&msg) {
  next_msg = std::move(msg);
}
MockClientReqHandler::MockClientReqHandler(
    std::function<void(const std::string &)> response_handler)
    : response_handler(std::move(response_handler)) {}
} // namespace k2cache