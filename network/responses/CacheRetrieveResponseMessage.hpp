//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CACHERETRIEVERESPONSEMESSAGE_HPP
#define RDFCACHEK2_CACHERETRIEVERESPONSEMESSAGE_HPP


#include <string>
#include <GraphResult.hpp>
#include "ResponseMessage.hpp"

class CacheRetrieveResponseMessage : public ResponseMessage{
  std::string serialized_result;
public:
  CacheRetrieveResponseMessage(GraphResult &result);

  char *get_response_buffer() override;

  int get_response_size() override;

};


#endif //RDFCACHEK2_CACHERETRIEVERESPONSEMESSAGE_HPP
