//
// Created by cristobal on 03-10-22.
//

#ifndef RDFCACHEK2_FICFACTORY_HPP
#define RDFCACHEK2_FICFACTORY_HPP

#include "FullyIndexedCache.hpp"
#include "manager/K2TreeFetcher.hpp"
#include <memory>
namespace k2cache::FICFactory{
  std::unique_ptr<FullyIndexedCache> create(std::unique_ptr<K2TreeFetcher> &&fetcher);
}


#endif // RDFCACHEK2_FICFACTORY_HPP
