//
// Created by Cristobal Miranda, 2020
//

#include "Cache.hpp"

Cache::Cache(){

}

bool Cache::result_exists(const std::string &label) {
  return results_map.find(label) != results_map.end() ;
}

GraphResult &Cache::get_graph_result(const std::string &label) {
  return results_map[label];
}
