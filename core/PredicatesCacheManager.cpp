//
// Created by Cristobal Miranda, 2020
//

#include "PredicatesCacheManager.hpp"
#include "K2TreeBulkOp.hpp"
#include <StringDictionary.h>
#include <chrono>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

PredicatesCacheManager::PredicatesCacheManager(
    std::unique_ptr<ISDManager> &&isd_manager,
    std::unique_ptr<PredicatesIndexCacheMDFile> &&predicates_index)
    : isd_manager(std::move(isd_manager)),
      predicates_index(std::move(predicates_index)),
      measured_time_sd_lookup(0) {}

PredicatesCacheManager::PredicatesCacheManager(
    std::unique_ptr<ISDManager> &&isd_manager, const std::string &fname)
    : PredicatesCacheManager(
          std::move(isd_manager),
          std::make_unique<PredicatesIndexCacheMDFile>(fname)) {}
uint64_t
PredicatesCacheManager::get_resource_index(const RDFResource &resource) const {
  auto index = get_resource_index_notfound_zero(resource);
  if (index == NORESULT) {
    auto naive_id = extra_dicts.locate_resource(resource);
    if (naive_id == NORESULT) {
      return NORESULT;
    }
    return naive_id + isd_manager->last_id();
  }

  return index;
}

uint64_t PredicatesCacheManager::get_resource_index_notfound_zero(
    const RDFResource &resource) const {
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

  return index;
}

void PredicatesCacheManager::handle_not_found(unsigned long &resource_id,
                                              RDFResource &resource) {
  if (resource_id != NORESULT)
    return;

  resource_id = extra_dicts.locate_resource(resource);
  if (resource_id == 0) {
    extra_dicts.add_resource(resource);
    resource_id = extra_dicts.locate_resource(resource);
  }
  resource_id = resource_id + isd_manager->last_id();
}

void PredicatesCacheManager::add_triple(RDFTripleResource &&rdf_triple) {
  add_triple(rdf_triple);
}
void PredicatesCacheManager::add_triple(RDFTripleResource &rdf_triple) {
  auto start = std::chrono::high_resolution_clock::now();
  auto subject_id = get_resource_index_notfound_zero(rdf_triple.subject);
  auto predicate_id = get_resource_index_notfound_zero(rdf_triple.predicate);
  auto object_id = get_resource_index_notfound_zero(rdf_triple.object);

  measured_time_sd_lookup +=
      std::chrono::duration_cast<std::chrono::nanoseconds>(
          std::chrono::high_resolution_clock::now() - start)
          .count();

  handle_not_found(subject_id, rdf_triple.subject);
  handle_not_found(predicate_id, rdf_triple.predicate);
  handle_not_found(object_id, rdf_triple.object);
  predicates_index->insert_point(subject_id, predicate_id, object_id);
}

void PredicatesCacheManager::save_all(const std::string &fname,
                                      const std::string &dirname) {

  // auto predicates_fname_path = fs::path(dirname)  / fs::path("k2ts-" +
  // fname);
  auto iris_fname_path = fs::path(dirname) / fs::path("iris-sd-" + fname);
  auto blanks_fname_path = fs::path(dirname) / fs::path("blanks-sd-" + fname);
  auto literals_fname_path =
      fs::path(dirname) / fs::path("literals-sd-" + fname);

  predicates_index->sync_file();

  isd_manager->save(iris_fname_path.string(), blanks_fname_path.string(),
                    literals_fname_path.string());
}

NaiveDynamicStringDictionary &PredicatesCacheManager::get_dyn_dicts() {
  return extra_dicts;
}

void PredicatesCacheManager::replace_index_cache(
    std::unique_ptr<PredicatesIndexCacheMDFile> &&predicates_index_input) {
  this->predicates_index = std::move(predicates_index_input);
}

PredicateFetchResult PredicatesCacheManager::get_tree_by_predicate_name(
    const std::string &predicate_name) const {
  RDFResource resource(predicate_name, RDF_TYPE_IRI);
  auto index = get_resource_index(resource);
  if (index == NORESULT)
    return PredicateFetchResult(false, nullptr);
  return predicates_index->fetch_k2tree(index);
}

PredicateFetchResult
PredicatesCacheManager::get_tree_by_predicate_index(unsigned long index) const {
  return predicates_index->fetch_k2tree(index);
}

unsigned long
PredicatesCacheManager::get_iri_index(const std::string &value) const {
  auto index = isd_manager->iris_index(value);
  if (index == 0) {
    RDFResource res(value, RDFResourceType::RDF_TYPE_IRI);
    return extra_dicts.locate_resource(res) + isd_manager->last_id();
  }
  return index;
}
unsigned long
PredicatesCacheManager::get_literal_index(const std::string &value) const {
  auto index = isd_manager->literals_index(value);
  if (index == 0) {
    RDFResource res(value, RDFResourceType::RDF_TYPE_LITERAL);
    return extra_dicts.locate_resource(res) + isd_manager->last_id();
  }
  return index;
}
unsigned long
PredicatesCacheManager::get_blank_index(const std::string &value) const {
  auto index = isd_manager->blanks_index(value);
  if (index == 0) {
    RDFResource res(value, RDFResourceType::RDF_TYPE_BLANK);
    return extra_dicts.locate_resource(res) + isd_manager->last_id();
  }
  return index;
}

RDFResource
PredicatesCacheManager::extract_resource(unsigned long index) const {
  if (index <= isd_manager->last_id())
    return isd_manager->get_resource(index);
  return extra_dicts.extract_resource(index - isd_manager->last_id());
}

bool PredicatesCacheManager::has_triple(
    const RDFTripleResource &rdf_triple) const {
  auto subject_index = get_resource_index(rdf_triple.subject);
  auto predicate_index = get_resource_index(rdf_triple.predicate);
  auto object_index = get_resource_index(rdf_triple.object);

  auto fetch_result = predicates_index->fetch_k2tree(predicate_index);
  if (!fetch_result.exists())
    return false;
  return fetch_result.get().has(subject_index, object_index);
}

PredicatesIndexCacheMDFile &
PredicatesCacheManager::get_predicates_index_cache() {
  return *predicates_index;
}

ISDManager *PredicatesCacheManager::get_isd_manager() {
  return isd_manager.get();
}

std::vector<std::pair<unsigned long, std::string>>
PredicatesCacheManager::get_plain_mapping_debug() {
  std::vector<std::pair<unsigned long, std::string>> result;
  for (unsigned long i = 1; i <= isd_manager->last_id() + extra_dicts.size();
       i++) {
    auto res = extract_resource(i);
    result.push_back({i, res.value});
  }
  return result;
}
unsigned long PredicatesCacheManager::get_last_id() const {
  return isd_manager->last_id() + extra_dicts.size();
}
bool PredicatesCacheManager::has_predicate(
    const std::string &predicate_name) const {
  auto index = isd_manager->iris_index(predicate_name);
  if (index == NORESULT) {
    RDFResource resource(predicate_name, RDFResourceType::RDF_TYPE_IRI);
    index = extra_dicts.locate_resource(resource);
  }
  if (index == NORESULT)
    return false;

  return predicates_index->has_predicate(index);
}
void PredicatesCacheManager::ensure_available_predicate(
    RDFResource predicate_resource) {
  auto id = get_resource_index(predicate_resource);
  if (id == 0)
    return;
  if (predicates_index->has_predicate(id)) {
    predicates_index->fetch_k2tree(id);
  }
}
void PredicatesCacheManager::load_all_predicates() {
  auto &metadata = predicates_index->get_metadata();
  for (auto predicate_id : metadata.get_ids_vector()) {
    predicates_index->fetch_k2tree(predicate_id);
  }
}
