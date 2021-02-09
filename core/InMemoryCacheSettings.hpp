#ifndef _IN_MEMORY_CACHE_SETTINGS_HPP_
#define _IN_MEMORY_CACHE_SETTINGS_HPP_

#include <sstream>
#include "ICacheSettings.hpp"

class InMemoryCacheSettings : public ICacheSettings {
  std::istringstream iss;
  public:
  explicit InMemoryCacheSettings(const std::string &settings);
  std::istream & get_predicates_input_stream() override;
};

#endif /* _IN_MEMORY_CACHE_SETTINGS_HPP_ */
