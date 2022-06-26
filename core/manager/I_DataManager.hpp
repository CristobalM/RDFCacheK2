//
// Created by cristobal on 04-08-21.
//

#ifndef RDFCACHEK2_I_DATAMANAGER_HPP
#define RDFCACHEK2_I_DATAMANAGER_HPP

namespace k2cache {
class I_DataManager {
public:
  virtual ~I_DataManager() = default;
  virtual void remove_key(unsigned long key) = 0;
  virtual void retrieve_key(unsigned long key) = 0;
};
} // namespace k2cache

#endif // RDFCACHEK2_I_DATAMANAGER_HPP
