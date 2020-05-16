//
// Created by Cristobal Miranda, 2020
//

#include "FeedData.hpp"

std::string FeedData::get_label() {
  return std::string();
}

FeedData::FeedData(std::string &query_label, char *data_buffer) : query_label(query_label) {
  triples_qty = *reinterpret_cast<uint64_t *>(data_buffer);
  this->data_buffer = data_buffer + sizeof(uint64_t);
}

FeedDataTripleIterator FeedData::begin() const {
  return FeedDataTripleIterator(reinterpret_cast<RDFTriple *>(data_buffer));
}

FeedDataTripleIterator FeedData::end() const {
  return FeedDataTripleIterator(reinterpret_cast<RDFTriple *>(data_buffer) + triples_qty);
}
