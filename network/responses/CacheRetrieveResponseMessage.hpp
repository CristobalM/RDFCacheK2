//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CACHERETRIEVERESPONSEMESSAGE_HPP
#define RDFCACHEK2_CACHERETRIEVERESPONSEMESSAGE_HPP

#include "ResponseMessage.hpp"
#include <GraphResult.hpp>
#include <string>

class CacheRetrieveResponseMessage : public ResponseMessage {
  std::string serialized_result;

public:
  CacheRetrieveResponseMessage(GraphResult &result);

  char *get_response_buffer() override;

  int get_response_size() override;
};

#endif // RDFCACHEK2_CACHERETRIEVERESPONSEMESSAGE_HPP
