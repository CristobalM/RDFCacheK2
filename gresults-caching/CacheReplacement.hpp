#ifndef _CACHE_REPLACEMENT_HPP_
#define _CACHE_REPLACEMENT_HPP_


class CacheReplacement{
  public:
  enum STRATEGY {
    LRU = 0,
  };

  virtual ~CacheReplacement(){}
};

#endif /* _CACHE_REPLACEMENT_HPP_ */

