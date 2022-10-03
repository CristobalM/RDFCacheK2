//
// Created by cristobal on 03-10-22.
//

#ifndef RDFCACHEK2_DATAMANAGERMOCK_HPP
#define RDFCACHEK2_DATAMANAGERMOCK_HPP

#include "manager/DataManager.hpp"
namespace k2cache{
class DataManagerMock : public DataManager {
public:
  void remove_key(unsigned long key) override;
  void retrieve_key(unsigned long key) override;
};

}

#endif // RDFCACHEK2_DATAMANAGERMOCK_HPP
