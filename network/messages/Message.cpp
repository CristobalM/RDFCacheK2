//
// Created by Cristobal Miranda, 2020
//

#include <iostream>
#include <netinet/in.h>
#include <utility>

#include "Message.hpp"
namespace k2cache {
Message::Message(uint32_t message_size)
    : message_size(message_size),
      buffer(std::make_unique<char[]>(message_size)) {}

proto_msg::MessageType Message::request_type() {
  return deserialized->request_type();
}

char *Message::get_buffer() { return buffer.get(); }

size_t Message::get_size() { return message_size; }

void Message::deserialize() {
  deserialized = std::make_unique<proto_msg::CacheRequest>();
  deserialized->ParseFromArray(buffer.get(), message_size);
}

proto_msg::CacheRequest &Message::get_cache_request() { return *deserialized; }
Message::Message(const Message &message) {
  message_size = message.message_size;
  buffer = std::make_unique<char[]>(message_size);
  std::copy(message.buffer.get(), message.buffer.get() + message_size,
            buffer.get());
  deserialized =
      std::make_unique<proto_msg::CacheRequest>(*message.deserialized);
}
Message &Message::operator=(const Message &message) {
  message_size = message.message_size;
  buffer = std::make_unique<char[]>(message_size);
  std::copy(message.buffer.get(), message.buffer.get() + message_size,
            buffer.get());
  deserialized =
      std::make_unique<proto_msg::CacheRequest>(*message.deserialized);
  return *this;
}
Message::Message(Message &&message) noexcept {
  message_size = message.message_size;
  buffer = std::move(message.buffer);
  deserialized = std::move(message.deserialized);
}
Message &Message::operator=(Message &&message) noexcept {
  message_size = message.message_size;
  buffer = std::move(message.buffer);
  deserialized = std::move(message.deserialized);
  return *this;
}
} // namespace k2cache
