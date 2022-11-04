//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_MESSAGE_HPP
#define RDFCACHEK2_MESSAGE_HPP

#include "request_msg.pb.h"
#include <cstdint>
#include <memory>
namespace k2cache {
class Message {
  uint32_t message_size;
  /**
   * Syntax:
   * uint32_t ReqType
   * the remaining message_size - 4 bytes depend on ReqType
   */
  std::unique_ptr<char[]> buffer;
  std::unique_ptr<proto_msg::CacheRequest> deserialized;

public:
  explicit Message(uint32_t message_size);
  Message(char *data, uint32_t message_size);

  Message(const Message& message);
  Message &operator=(const Message &message);
  Message(Message&& message) noexcept;
  Message &operator=(Message &&message) noexcept;

  proto_msg::MessageType request_type();

  enum ReqType { CACHE_CHECK = 0, CACHE_FEED, CACHE_RETRIEVE, CONNECTION_END };

  char *get_buffer();

  size_t get_size();

  void deserialize();

  proto_msg::CacheRequest &get_cache_request();

};
} // namespace k2cache
#endif // RDFCACHEK2_MESSAGE_HPP
