#ifndef _CACHE_SETTINGS_HPP_
#define _CACHE_SETTINGS_HPP_

#include <fstream>
#include <memory>

#include "ICacheSettings.hpp"
#include "CacheConfig.hpp"

struct CacheSettings : public ICacheSettings{
  std::unique_ptr<std::ifstream> predicates_file;

  public:

  explicit CacheSettings(CacheConfig &config);

  std::istream & get_predicates_input_stream() override;
};

#endif /* _CACHE_SETTINGS_HPP_ */
