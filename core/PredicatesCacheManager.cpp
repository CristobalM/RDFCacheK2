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
    std::cerr << "Subject " << subject << " does not exist" << std::endl;
    subject_id = extra_dicts.locate_subject(subject);
    if (subject_id == 0) {
      extra_dicts.add_subject(subject);
      subject_id = extra_dicts.locate_subject(subject);
    }
    subject_id = subject_id + isd_manager->last_subject_id() - 1;
  }

  if (predicate_id == NORESULT) {
    std::cerr << "Predicate " << predicate << " does not exist" << std::endl;
    predicate_id = extra_dicts.locate_predicate(predicate);
    if (predicate_id == 0) {
      extra_dicts.add_predicate(predicate);
      predicate_id = extra_dicts.locate_predicate(predicate);
    }
    predicate_id = predicate_id + isd_manager->last_predicate_id() - 1;
  }

  if (object_id == NORESULT) {
    std::cerr << "Object " << object << " does not exist" << std::endl;
    object_id = extra_dicts.locate_object(object);
    if (object_id == 0) {
      extra_dicts.add_object(object);
      object_id = extra_dicts.locate_object(object);
    }
    object_id = object_id + isd_manager->last_object_id() - 1;
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

void PredicatesCacheManager::save_all(const std::string &fname) {
  predicates_index->dump_to_file("k2ts-" + fname);
  isd_manager->save("subjects-sd-" + fname, "predicates-sd-" + fname,
                    "objects-sd-" + fname);
}

NaiveDynamicStringDictionary &PredicatesCacheManager::get_dyn_dicts() {
  return extra_dicts;
}
