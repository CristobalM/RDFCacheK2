//
// Created by cristobal on 04-08-21.
//

#ifndef RDFCACHEK2_DATAMANAGER_HPP
#define RDFCACHEK2_DATAMANAGER_HPP

#include <cstdint>

namespace k2cache {
class DataManager {
public:
  virtual ~DataManager() = default;
  virtual void remove_key(uint64_t key) = 0;
  virtual void retrieve_key(uint64_t key) = 0;
};
} // namespace k2cache

#endif // RDFCACHEK2_DATAMANAGER_HPP
