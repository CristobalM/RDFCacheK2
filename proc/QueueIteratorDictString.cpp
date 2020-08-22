//
// Created by Cristobal Miranda, 2020
//

#include "QueueIteratorDictString.hpp"

#include <exception>

bool QueueIteratorDictString::hasNext() { return !source_feed->has_finished(); }

unsigned char *QueueIteratorDictString::next(uint *str_length) {
  auto next_value = source_feed->get_string();
  if (next_value == FINISHED_VAL) {
    *str_length = 0;
    throw std::runtime_error("CHUCHA");
    return EMPTY_STRING;
  }

  auto *result = new unsigned char[next_value.size() + 1];
  mempcpy(result, next_value.data(), next_value.size());
  result[next_value.size()] = 0;
  *str_length = next_value.size();

  return result;
}

QueueIteratorDictString::QueueIteratorDictString(SDFeed *source_feed)
    : source_feed(source_feed) {}

QueueIteratorDictString::~QueueIteratorDictString() {}
