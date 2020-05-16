//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_MESSAGE_HPP
#define RDFCACHEK2_MESSAGE_HPP


#include <memory>
#include <cstdint>
#include <FeedData.hpp>

class Message {
  uint32_t message_size;
  /**
   * Syntax:
   * uint32_t ReqType
   * the remaining message_size - 4 bytes depend on ReqType
   */
  std::unique_ptr<char[]> buffer;
public:
  Message(std::unique_ptr<char[]> &&buffer, uint32_t message_size);

  Message(Message &&);
  Message &operator=(Message &&);

  int request_type();
  std::string get_query_label();
  FeedData get_feed_data();


  enum ReqType{
    CACHE_CHECK = 0,
    CACHE_FEED,
    CACHE_RETRIEVE
  };

private:
  Message(const Message&);
  Message &operator=(const Message &);
};


#endif //RDFCACHEK2_MESSAGE_HPP
