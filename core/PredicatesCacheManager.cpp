//
// Created by Cristobal Miranda, 2020
//

#include "PredicatesCacheManager.hpp"
#include <StringDictionary.h>
#include <chrono>

PredicatesCacheManager::PredicatesCacheManager(
    std::unique_ptr<ISDManager> &&isd_manager,
    std::unique_ptr<PredicatesIndexCache> &&predicates_index)
    : isd_manager(std::move(isd_manager)),
      predicates_index(std::move(predicates_index)),
      measured_time_sd_lookup(0) {}

uint64_t PredicatesCacheManager::get_resource_index(RDFResource &resource) {
  switch (resource.resource_type) {
  case RDF_TYPE_IRI:
    return isd_manager->iris_index(resource.value);
  case RDF_TYPE_BLANK:
    return isd_manager->blanks_index(resource.value);
  case RDF_TYPE_LITERAL:
    return isd_manager->literals_index(resource.value);
  }
  return NORESULT;
}

void PredicatesCacheManager::handle_not_found(unsigned long &resource_id,
                                              RDFResource &resource) {
  if (resource_id == NORESULT) {
    std::string res_type_name;
    switch (resource.resource_type) {
    case RDF_TYPE_IRI:
      res_type_name = "IRI";
      break;
    case RDF_TYPE_BLANK:
      res_type_name = "BLANK_NODE";
      break;
    case RDF_TYPE_LITERAL:
      res_type_name = "LITERAL";
      break;
    default:
      res_type_name = "UNKNOWN";
    }
    std::cerr << "Resource " << resource.value << " of type " << res_type_name
              << " does not exist" << std::endl;
    resource_id = extra_dicts.locate_resource(resource);
    if (resource_id == 0) {
      extra_dicts.add_resource(resource);
      resource_id = extra_dicts.locate_resource(resource);
    }
    resource_id = resource_id + isd_manager->last_id();
  }
}

void PredicatesCacheManager::add_triple(RDFTripleResource &rdf_triple) {
  auto start = std::chrono::high_resolution_clock::now();
  auto subject_id = get_resource_index(rdf_triple.subject);
  auto predicate_id = get_resource_index(rdf_triple.predicate);
  auto object_id = get_resource_index(rdf_triple.object);

  handle_not_found(subject_id, rdf_triple.subject);
  handle_not_found(predicate_id, rdf_triple.predicate);
  handle_not_found(object_id, rdf_triple.object);

  measured_time_sd_lookup +=
      std::chrono::duration_cast<std::chrono::nanoseconds>(
          std::chrono::high_resolution_clock::now() - start)
          .count();

  if (!predicates_index->has_predicate(predicate_id)) {
    predicates_index->add_predicate(predicate_id);
  }
  predicates_index->get_k2tree(predicate_id).insert(subject_id, object_id);

  // predicates_index->insert_point(subject_id, predicate_id, object_id);
}

void PredicatesCacheManager::add_triple(RDFTripleResource &rdf_triple,
                                        PredicatesIndexCacheBuilder &builder) {
  auto start = std::chrono::high_resolution_clock::now();
  auto subject_id = get_resource_index(rdf_triple.subject);
  auto predicate_id = get_resource_index(rdf_triple.predicate);
  auto object_id = get_resource_index(rdf_triple.object);

  handle_not_found(subject_id, rdf_triple.subject);
  handle_not_found(predicate_id, rdf_triple.predicate);
  handle_not_found(object_id, rdf_triple.object);

  measured_time_sd_lookup +=
      std::chrono::duration_cast<std::chrono::nanoseconds>(
          std::chrono::high_resolution_clock::now() - start)
          .count();

  builder.insert_point(subject_id, predicate_id, object_id);
}

PredicatesIndexCache &PredicatesCacheManager::get_predicates_cache() {
  return *predicates_index;
}

PredicatesCacheManager::PredicatesCacheManager(
    std::unique_ptr<ISDManager> &&isd_manager)
    : PredicatesCacheManager(std::move(isd_manager),
                             std::make_unique<PredicatesIndexCache>()) {}

void PredicatesCacheManager::save_all(const std::string &fname) {
  predicates_index->dump_to_file("k2ts-" + fname);
  isd_manager->save("iris-sd-" + fname, "blanks-sd-" + fname,
                    "literals-sd-" + fname);
}

NaiveDynamicStringDictionary &PredicatesCacheManager::get_dyn_dicts() {
  return extra_dicts;
}

void PredicatesCacheManager::replace_index_cache(
    std::unique_ptr<PredicatesIndexCache> &&predicates_index) {
  this->predicates_index = std::move(predicates_index);
}
