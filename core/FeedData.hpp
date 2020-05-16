//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_FEEDDATA_HPP
#define RDFCACHEK2_FEEDDATA_HPP

#include <string>
#include <iterator>
#include <vector>
#include <string>
#include "RDFTriple.hpp"
#include "FeedDataTripleIterator.hpp"

class FeedData {
  std::string query_label;
  uint64_t triples_qty;
  char *data_buffer;
public:

  explicit FeedData(std::string &query_label, char *data_buffer);

  std::string get_label();

  FeedDataTripleIterator begin() const;
  FeedDataTripleIterator end() const;
};


#endif //RDFCACHEK2_FEEDDATA_HPP
