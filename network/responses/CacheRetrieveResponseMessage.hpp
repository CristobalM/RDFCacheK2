//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CACHERETRIEVERESPONSEMESSAGE_HPP
#define RDFCACHEK2_CACHERETRIEVERESPONSEMESSAGE_HPP


#include <GraphResult.hpp>
#include "ResponseMessage.hpp"

class CacheRetrieveResponseMessage : public ResponseMessage{
public:
  CacheRetrieveResponseMessage(GraphResult &result);

  char *get_response_buffer() override;

  int get_response_size() override;

};


#endif //RDFCACHEK2_CACHERETRIEVERESPONSEMESSAGE_HPP
