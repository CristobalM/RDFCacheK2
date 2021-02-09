#ifndef _I_CACHE_SETTINGS_HPP_
#define _I_CACHE_SETTINGS_HPP_

#include <istream>


class ICacheSettings{
  public:
  virtual std::istream & get_predicates_input_stream() = 0;

  virtual ~ICacheSettings(){}
};

#endif /* */
