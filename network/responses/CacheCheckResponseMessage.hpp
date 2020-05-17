//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CACHECHECKRESPONSEMESSAGE_HPP
#define RDFCACHEK2_CACHECHECKRESPONSEMESSAGE_HPP

#include "ResponseMessage.hpp"
#include <array>
#include <cstdint>

class CacheCheckResponseMessage : public ResponseMessage {
  std::array<uint32_t, 2> response;

public:
  explicit CacheCheckResponseMessage(bool exists);

  char *get_response_buffer() override;

  int get_response_size() override;
};

#endif // RDFCACHEK2_CACHECHECKRESPONSEMESSAGE_HPP
