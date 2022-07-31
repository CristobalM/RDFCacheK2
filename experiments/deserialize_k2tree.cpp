//
// Created by cristobal on 27-07-22.
//

#include "k2tree/K2TreeBulkOp.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <k2tree/K2TreeMixed.hpp>
#include <stdexcept>
#include <string>

using namespace k2cache;
int main(int argc, char **argv) {
  if (argc < 3) {
    throw std::runtime_error("was expecting a file argument and output");
  }
  auto fname = std::string(argv[1]);
  auto out_fname = std::string(argv[2]);

  std::ifstream ifs(fname, std::ios::binary | std::ios::in);
  std::ofstream ofs(out_fname);

  auto tree = K2TreeMixed::read_from_istream(ifs);
  ifs.close();

  auto scanner = tree.create_full_scanner();
  auto start = std::chrono::high_resolution_clock::now();
  while (scanner->has_next()) {
    auto pair = scanner->next();
    ofs << pair.first << " " << pair.second << "\n";
  }
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
  std::cout << "full scan and writing took in total " << duration.count()
            << " ms" << std::endl;
}
