//
// Created by Cristobal Miranda, 2020
//

#include "CacheCheckResponseMessage.hpp"
#include <Message.hpp>

char *CacheCheckResponseMessage::get_response_buffer() {
  return reinterpret_cast<char *>(response.data());
}

int CacheCheckResponseMessage::get_response_size() {
  return response.size() * sizeof(uint32_t);
}

CacheCheckResponseMessage::CacheCheckResponseMessage(bool exists) {
  response[0] = Message::ReqType::CACHE_CHECK;
  response[1] = (uint32_t)exists;
}
