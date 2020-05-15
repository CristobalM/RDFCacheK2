//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CANTESTABLISHSOCKETEXCEPTION_HPP
#define RDFCACHEK2_CANTESTABLISHSOCKETEXCEPTION_HPP

#include <stdexcept>
#include <string>

class CantEstablishSocketException : std::runtime_error{
public:
  CantEstablishSocketException(uint16_t port) :
  std::runtime_error("Cant establish a socket with port " + std::to_string(port))  {}
};
#endif //RDFCACHEK2_CANTESTABLISHSOCKETEXCEPTION_HPP
