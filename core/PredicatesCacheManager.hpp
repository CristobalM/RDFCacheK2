//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_PREDICATESCACHEMANAGER_HPP
#define RDFCACHEK2_PREDICATESCACHEMANAGER_HPP

#include "ISDManager.hpp"
#include "NaiveDynamicStringDictionary.hpp"
#include "PredicatesIndexCache.hpp"
#include "RDFTriple.hpp"
#include <memory>
#include <string>

class PredicatesCacheManager {
  std::unique_ptr<ISDManager> isd_manager;
  std::unique_ptr<PredicatesIndexCache> predicates_index;

  NaiveDynamicStringDictionary extra_dicts;

public:
  double measured_time_sd_lookup;

  static constexpr int DEFAULT_WORKER_POOL_SZ = 4;
  static constexpr unsigned long DEFAULT_MAX_QUEUE_SIZE = 10'000'000;

  PredicatesCacheManager(
      std::unique_ptr<ISDManager> &&isd_manager,
      std::unique_ptr<PredicatesIndexCache> &&predicates_index);

  explicit PredicatesCacheManager(std::unique_ptr<ISDManager> &&isd_manager);

  void add_triple(RDFTripleResource &rdf_triple);
  void add_triple(RDFTripleResource &&rdf_triple);
  
  void add_triple(RDFTripleResource &rdf_triple,
                  PredicatesIndexCacheBuilder &builder);

  void
  replace_index_cache(std::unique_ptr<PredicatesIndexCache> &&predicates_index);

  PredicatesIndexCache &get_predicates_cache();
  NaiveDynamicStringDictionary &get_dyn_dicts();

  void save_all(const std::string &fname);

private:
  uint64_t get_resource_index(RDFResource &resource);
  void handle_not_found(unsigned long &resource_id, RDFResource &resource);
};

#endif // RDFCACHEK2_PREDICATESCACHEMANAGER_HPP
