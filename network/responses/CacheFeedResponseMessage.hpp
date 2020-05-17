//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CACHEFEEDRESPONSEMESSAGE_HPP
#define RDFCACHEK2_CACHEFEEDRESPONSEMESSAGE_HPP

#include "ResponseMessage.hpp"
#include <array>
#include <cstdint>

class CacheFeedResponseMessage : public ResponseMessage {
  std::array<uint32_t, 2> response;

public:
  CacheFeedResponseMessage(bool feed_result);

  char *get_response_buffer() override;

  int get_response_size() override;
};

#endif // RDFCACHEK2_CACHEFEEDRESPONSEMESSAGE_HPP
