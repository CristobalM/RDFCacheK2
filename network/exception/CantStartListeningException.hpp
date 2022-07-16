//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CANTSTARTLISTENINGEXCEPTION_HPP
#define RDFCACHEK2_CANTSTARTLISTENINGEXCEPTION_HPP
namespace k2cache {
class CantStartListeningException : std::runtime_error {
public:
  CantStartListeningException(uint16_t port)
      : std::runtime_error("Cant start listening port " +
                           std::to_string(port)) {}
};
} // namespace k2cache
#endif // RDFCACHEK2_CANTSTARTLISTENINGEXCEPTION_HPP
