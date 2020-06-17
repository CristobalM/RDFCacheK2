//
// Created by Cristobal Miranda, 2020
//

#include "PredicatesCacheManager.hpp"
#include <StringDictionary.h>

PredicatesCacheManager::PredicatesCacheManager(
    std::unique_ptr<ISDManager> &&isd_manager,
    std::unique_ptr<PredicatesIndexCache> &&predicates_index)
    : isd_manager(std::move(isd_manager)),
      predicates_index(std::move(predicates_index)) {}

void PredicatesCacheManager::add_triple(std::string &subject,
                                        std::string &predicate,
                                        std::string &object) {
  auto subject_id = isd_manager->subject_index(subject);
  auto predicate_id = isd_manager->predicate_index(predicate);
  auto object_id = isd_manager->object_index(object);

  if (subject_id == NORESULT) {
    throw std::runtime_error("Subject " + subject + " does not exist");
  }

  if (predicate_id == NORESULT) {
    throw std::runtime_error("Predicate " + predicate + " does not exist");
  }

  if (object_id == NORESULT) {
    throw std::runtime_error("Object " + object + " does not exist");
  }

  if (!predicates_index->has_predicate(predicate_id)) {
    predicates_index->add_predicate(predicate_id);
  }

  auto &k2tree = predicates_index->get_k2tree(predicate_id);
  k2tree.insert(subject_id, object_id);
}

PredicatesIndexCache &PredicatesCacheManager::get_predicates_cache() {
  return *predicates_index;
}

PredicatesCacheManager::PredicatesCacheManager(
    std::unique_ptr<ISDManager> &&isd_manager)
    : PredicatesCacheManager(std::move(isd_manager),
                             std::make_unique<PredicatesIndexCache>()) {}
