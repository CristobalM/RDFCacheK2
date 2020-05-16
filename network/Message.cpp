//
// Created by Cristobal Miranda, 2020
//

#include <utility>

#include "Message.hpp"

Message::Message(std::unique_ptr<char[]> &&buffer, uint32_t message_size) :
message_size(message_size),
buffer(std::move(buffer))
{}

Message::Message(Message &&other) :
message_size(other.message_size),
buffer(std::move(other.buffer))
{}

Message &Message::operator=(Message &&rhs) {
  message_size = rhs.message_size;
  buffer= std::move(rhs.buffer);
  return *this;
}

int Message::request_type() {
  return *reinterpret_cast<uint32_t *>(buffer.get());
}

std::string Message::get_query_label() {
  auto label_sz = *reinterpret_cast<uint32_t *>(buffer.get() + sizeof(uint32_t));
  char *data = buffer.get() + sizeof(uint32_t) * 2;
  return std::string(data, data + label_sz * sizeof(char));
}
