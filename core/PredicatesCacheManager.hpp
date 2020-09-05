//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_PREDICATESCACHEMANAGER_HPP
#define RDFCACHEK2_PREDICATESCACHEMANAGER_HPP

#include "ISDManager.hpp"
#include "NaiveDynamicStringDictionary.hpp"
#include "PredicatesIndexCache.hpp"
#include <memory>
#include <string>

class PredicatesCacheManager {
  std::unique_ptr<ISDManager> isd_manager;
  std::unique_ptr<PredicatesIndexCache> predicates_index;

  NaiveDynamicStringDictionary extra_dicts;

public:
  PredicatesCacheManager(
      std::unique_ptr<ISDManager> &&isd_manager,
      std::unique_ptr<PredicatesIndexCache> &&predicates_index);

  explicit PredicatesCacheManager(std::unique_ptr<ISDManager> &&isd_manager);

  void add_triple(std::string &subject, std::string &predicate,
                  std::string &object);

  PredicatesIndexCache &get_predicates_cache();
  NaiveDynamicStringDictionary &get_dyn_dicts();

  void save_all(const std::string &fname);
};

#endif // RDFCACHEK2_PREDICATESCACHEMANAGER_HPP
