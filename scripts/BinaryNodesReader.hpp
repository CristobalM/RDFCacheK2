//
// Created by cristobal on 20-11-21.
//

#ifndef RDFCACHEK2_BINARYNODESREADER_HPP
#define RDFCACHEK2_BINARYNODESREADER_HPP

#include <fstream>

class BinaryNodesReader {
  std::ifstream &ifs;
  unsigned long nodes;

public:
  explicit BinaryNodesReader(std::ifstream &ifs) : ifs(ifs) {
    nodes = read_u64(ifs);
  }

  void read_value(ExternalSort::UnsignedLongSortConnector &value) {}
};

#endif // RDFCACHEK2_BINARYNODESREADER_HPP
