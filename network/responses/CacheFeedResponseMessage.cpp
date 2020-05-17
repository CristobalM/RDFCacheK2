//
// Created by Cristobal Miranda, 2020
//

#include "CacheFeedResponseMessage.hpp"
#include <Message.hpp>

char *CacheFeedResponseMessage::get_response_buffer() { return nullptr; }

int CacheFeedResponseMessage::get_response_size() { return 0; }

CacheFeedResponseMessage::CacheFeedResponseMessage(bool feed_result) {
  response[0] = Message::ReqType::CACHE_FEED;
  response[1] = feed_result;
}
