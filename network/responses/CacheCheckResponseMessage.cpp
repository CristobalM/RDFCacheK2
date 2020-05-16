//
// Created by Cristobal Miranda, 2020
//

#include "CacheCheckResponseMessage.hpp"

char *CacheCheckResponseMessage::get_response_buffer() {
  return nullptr;
}

int CacheCheckResponseMessage::get_response_size() {
  return 0;
}

CacheCheckResponseMessage::CacheCheckResponseMessage(bool exists) : exists(exists) {

}
