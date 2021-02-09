#include <stdexcept>
#include <fstream>

#include "CacheSettings.hpp"

std::istream & CacheSettings::get_predicates_input_stream(){
  if(!predicates_file){
    throw std::runtime_error("CacheSettings::get_predicates_input_stream: Predicates file was not initialized");
  }
  return *predicates_file;
}

CacheSettings::CacheSettings(CacheConfig &config){
  predicates_file = std::make_unique<std::ifstream>(config.cache_file_path);
}
