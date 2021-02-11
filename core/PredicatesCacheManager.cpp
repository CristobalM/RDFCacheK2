//
// Created by Cristobal Miranda, 2020
//

#include "PredicatesCacheManager.hpp"
#include <StringDictionary.h>
#include <chrono>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;


PredicatesCacheManager::PredicatesCacheManager(
    std::unique_ptr<ISDManager> &&isd_manager,
    std::unique_ptr<PredicatesIndexCacheMDFile> &&predicates_index,
    K2TreeConfig k2tree_config)
    : isd_manager(std::move(isd_manager)),
      predicates_index(std::move(predicates_index)),
      k2tree_config(std::move(k2tree_config)),
      measured_time_sd_lookup(0)
       {}

PredicatesCacheManager::PredicatesCacheManager(
    std::unique_ptr<ISDManager> &&isd_manager, K2TreeConfig k2tree_config, const std::string &fname)
    : PredicatesCacheManager(std::move(isd_manager),
                             std::make_unique<PredicatesIndexCacheMDFile>(fname, k2tree_config), k2tree_config) {}
uint64_t
PredicatesCacheManager::get_resource_index(const RDFResource &resource) const {
  unsigned long index;
  switch (resource.resource_type) {
  case RDF_TYPE_IRI:
    index = isd_manager->iris_index(resource.value);
    break;
  case RDF_TYPE_BLANK:
    index = isd_manager->blanks_index(resource.value);
    break;
  case RDF_TYPE_LITERAL:
    index = isd_manager->literals_index(resource.value);
    break;
  default:
    index = NORESULT;
    break;
  }

  if (index == NORESULT)
    return extra_dicts.locate_resource(resource.value) + isd_manager->last_id();

  return index;
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
      throw std::runtime_error("Unknown resource type: '" +
                               std::to_string(resource.resource_type) +
                               "' with value '" + resource.value + "'");
    }

    resource_id = extra_dicts.locate_resource(resource.value);
    if (resource_id == 0) {
      extra_dicts.add_resource(resource.value);
      resource_id = extra_dicts.locate_resource(resource.value);
    }
    resource_id = resource_id + isd_manager->last_id();
  }
}
void PredicatesCacheManager::add_triple(RDFTripleResource &&rdf_triple) {
  add_triple(rdf_triple);
}
void PredicatesCacheManager::add_triple(RDFTripleResource &rdf_triple) {
  auto start = std::chrono::high_resolution_clock::now();
  auto subject_id = get_resource_index(rdf_triple.subject);
  auto predicate_id = get_resource_index(rdf_triple.predicate);
  auto object_id = get_resource_index(rdf_triple.object);

  measured_time_sd_lookup +=
      std::chrono::duration_cast<std::chrono::nanoseconds>(
          std::chrono::high_resolution_clock::now() - start)
          .count();

  handle_not_found(subject_id, rdf_triple.subject);
  handle_not_found(predicate_id, rdf_triple.predicate);
  handle_not_found(object_id, rdf_triple.object);
  predicates_index->insert_point(subject_id, predicate_id, object_id);
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

void PredicatesCacheManager::save_all(const std::string &fname, const std::string &dirname) {

  // auto predicates_fname_path = fs::path(dirname)  / fs::path("k2ts-" + fname);
  auto iris_fname_path = fs::path(dirname)  / fs::path("iris-sd-" + fname);
  auto blanks_fname_path = fs::path(dirname)  / fs::path("blanks-sd-" + fname);
  auto literals_fname_path = fs::path(dirname)  / fs::path("literals-sd-" + fname);

  predicates_index->sync_file();

  isd_manager->save(iris_fname_path.string(), blanks_fname_path.string(),
                    literals_fname_path.string());
}

NaiveDynamicStringDictionary &PredicatesCacheManager::get_dyn_dicts() {
  return extra_dicts;
}

void PredicatesCacheManager::replace_index_cache(
    std::unique_ptr<PredicatesIndexCacheMDFile> &&predicates_index) {
  this->predicates_index = std::move(predicates_index);
}

K2TreeMixed &PredicatesCacheManager::get_tree_by_predicate_name(
    const std::string &predicate_name) {
  RDFResource resource(predicate_name, RDF_TYPE_IRI);
  auto index = get_resource_index(resource);
  if (index == NORESULT)
    throw std::runtime_error("Predicate with name " + predicate_name +
                             " was not found in predicates cache manager");
  return predicates_index->fetch_k2tree(index);
}

K2TreeMixed &
PredicatesCacheManager::get_tree_by_predicate_index(unsigned long index) {
  return predicates_index->fetch_k2tree(index);
}

unsigned long PredicatesCacheManager::get_iri_index(const std::string &value) {
  auto index = isd_manager->iris_index(value);
  if (index == 0) {
    return extra_dicts.locate_resource(value) + isd_manager->last_id();
  }
  return index;
}
unsigned long
PredicatesCacheManager::get_literal_index(const std::string &value) {
  auto index = isd_manager->literals_index(value);
  if (index == 0) {
    return extra_dicts.locate_resource(value) + isd_manager->last_id();
  }
  return index;
}
unsigned long
PredicatesCacheManager::get_blank_index(const std::string &value) {
  auto index = isd_manager->blanks_index(value);
  if (index == 0) {
    return extra_dicts.locate_resource(value) + isd_manager->last_id();
  }
  return index;
}

std::string
PredicatesCacheManager::extract_resource(unsigned long index) const {
  if (index <= isd_manager->last_id())
    return isd_manager->get_resource(index).value;
  return extra_dicts.extract_resource(index - isd_manager->last_id());
}

bool PredicatesCacheManager::has_triple(
    const RDFTripleResource &rdf_triple) const {
  auto subject_index = get_resource_index(rdf_triple.subject);
  auto predicate_index = get_resource_index(rdf_triple.predicate);
  auto object_index = get_resource_index(rdf_triple.object);

  auto &k2tree = predicates_index->fetch_k2tree(predicate_index);
  return k2tree.has(subject_index, object_index);
}

PredicatesIndexCacheMDFile &PredicatesCacheManager::get_predicates_index_cache() {
  return *predicates_index;
}

