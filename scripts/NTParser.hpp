//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_NTPARSER_HPP
#define RDFCACHEK2_NTPARSER_HPP

#include "EntitiesMapping.h"
#include <PredicatesIndexCache.hpp>
#include <memory>
#include <string>
#include <unordered_map>

struct NTParsedResult {
  NTParsedResult();
  NTParsedResult(EntitiesMapping mapping, PredicatesIndexCache cache);
  EntitiesMapping entities_mapping;
  PredicatesIndexCache predicates_index_cache;
};

class NTParser {
  std::string input_path;
  EntitiesMapping *previous_mapping = nullptr;

  std::unordered_map<std::string, bool> predicates;
  bool has_predicates = false;

public:
  NTParser(std::string input_path, EntitiesMapping *previous_mapping);
  NTParser(std::string input_path);

  std::unique_ptr<NTParsedResult> parse();
  void set_predicates(std::vector<std::string> &predicates_vec);

  bool should_add_predicate(const std::string &predicate);
};

#endif // RDFCACHEK2_NTPARSER_HPP
