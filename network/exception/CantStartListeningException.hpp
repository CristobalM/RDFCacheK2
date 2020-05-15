//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CANTSTARTLISTENINGEXCEPTION_HPP
#define RDFCACHEK2_CANTSTARTLISTENINGEXCEPTION_HPP

class CantStartListeningException : std::runtime_error {
public:
  CantStartListeningException(uint16_t port) :
  std::runtime_error("Cant start listening port " + std::to_string(port))  {}
};

#endif //RDFCACHEK2_CANTSTARTLISTENINGEXCEPTION_HPP
