//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_PREDICATESCACHEMANAGER_HPP
#define RDFCACHEK2_PREDICATESCACHEMANAGER_HPP

#include "ISDManager.hpp"
#include "NaiveDynamicStringDictionary.hpp"
#include "PredicatesIndexCacheMDFile.hpp"
#include "RDFTriple.hpp"
#include <istream>
#include <memory>
#include <string>

class PredicatesCacheManager {
  std::unique_ptr<ISDManager> isd_manager;
  std::unique_ptr<PredicatesIndexCacheMDFile> predicates_index;

  NaiveDynamicStringDictionary extra_dicts;

  K2TreeConfig k2tree_config;

public:
  double measured_time_sd_lookup;

  static constexpr int DEFAULT_WORKER_POOL_SZ = 4;
  static constexpr unsigned long DEFAULT_MAX_QUEUE_SIZE = 10'000'000;

  PredicatesCacheManager(
      std::unique_ptr<ISDManager> &&isd_manager,
      std::unique_ptr<PredicatesIndexCacheMDFile> &&predicates_index);

  PredicatesCacheManager(std::unique_ptr<ISDManager> &&isd_manager,
                         const std::string &fname);

  void add_triple(RDFTripleResource &rdf_triple);
  void add_triple(RDFTripleResource &&rdf_triple);

  bool has_triple(const RDFTripleResource &rdf_triple) const;

  void replace_index_cache(
      std::unique_ptr<PredicatesIndexCacheMDFile> &&predicates_index);

  PredicatesIndexCacheMDFile &get_predicates_cache();

  K2TreeMixed &get_tree_by_predicate_name(const std::string &predicate_name);
  K2TreeMixed &get_tree_by_predicate_index(unsigned long index);

  NaiveDynamicStringDictionary &get_dyn_dicts();

  void save_all(const std::string &fname, const std::string &dirname);

  unsigned long get_iri_index(const std::string &value);
  unsigned long get_literal_index(const std::string &value);
  unsigned long get_blank_index(const std::string &value);
  RDFResource extract_resource(unsigned long index) const;

  PredicatesIndexCacheMDFile &get_predicates_index_cache();

  uint64_t get_resource_index(const RDFResource &resource) const;

private:
  void handle_not_found(unsigned long &resource_id, RDFResource &resource);
  uint64_t get_resource_index_notfound_zero(const RDFResource &resource) const;
};

#endif // RDFCACHEK2_PREDICATESCACHEMANAGER_HPP
