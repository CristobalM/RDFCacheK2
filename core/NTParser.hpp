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
  explicit NTParsedResult(std::shared_ptr<EntitiesMapping> mapping);
  std::shared_ptr<EntitiesMapping> entities_mapping;
  std::unique_ptr<PredicatesIndexCache> predicates_index_cache;
};

class NTParser {
  std::string input_path;
  std::shared_ptr<EntitiesMapping> previous_mapping = nullptr;

  std::unordered_map<std::string, bool> predicates;
  bool has_predicates = false;

  std::vector<std::pair<ulong, ulong>> points_inserted_debug;

public:
  NTParser(std::string input_path,
           std::shared_ptr<EntitiesMapping> previous_mapping);
  NTParser(std::string input_path);

  std::unique_ptr<NTParsedResult> parse();
  void set_predicates(std::vector<std::string> &predicates_vec);

  bool should_add_predicate(const std::string &predicate);

  void insert_debug(ulong col, ulong row);

  std::vector<std::pair<ulong, ulong>> &get_pairs();
};

#endif // RDFCACHEK2_NTPARSER_HPP
