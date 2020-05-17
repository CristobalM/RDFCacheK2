//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_RESPONSEMESSAGE_HPP
#define RDFCACHEK2_RESPONSEMESSAGE_HPP

class ResponseMessage {
public:
  virtual char *get_response_buffer() = 0;
  virtual int get_response_size() = 0;
};

#endif // RDFCACHEK2_RESPONSEMESSAGE_HPP
