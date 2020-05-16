//
// Created by Cristobal Miranda, 2020
//

#include "CacheRetrieveResponseMessage.hpp"

char *CacheRetrieveResponseMessage::get_response_buffer() {
  return serialized_result.data();
}

int CacheRetrieveResponseMessage::get_response_size() {
  return serialized_result.size() * sizeof(char);
}

CacheRetrieveResponseMessage::CacheRetrieveResponseMessage(GraphResult &result) :
serialized_result(result.serialize_result()) {
}
