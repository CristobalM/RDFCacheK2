//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_QUEUEITERATORDICTSTRING_HPP
#define RDFCACHEK2_QUEUEITERATORDICTSTRING_HPP
#include "SDFeed.hpp"
#include <iterators/IteratorDictString.h>

static unsigned char EMPTY_STRING[] = "\0";

class QueueIteratorDictString : public IteratorDictString {
  SDFeed *source_feed;

public:

  virtual ~QueueIteratorDictString() override;

  explicit QueueIteratorDictString(SDFeed *source_feed);

  bool hasNext() override;

  unsigned char *next(uint *str_length) override;
};

#endif // RDFCACHEK2_QUEUEITERATORDICTSTRING_HPP
