#include "InMemoryCacheSettings.hpp"

InMemoryCacheSettings::InMemoryCacheSettings(const std::string &settings) : iss(settings){
}

std::istream & InMemoryCacheSettings::get_predicates_input_stream() {
  return iss;
}
