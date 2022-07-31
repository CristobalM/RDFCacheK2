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

  K2TreeConfig config{};
  config.treedepth = 32;
  config.cut_depth = 10;
  config.max_node_count = 1024;

  K2TreeMixed k2tree(config);
  K2TreeBulkOp op(k2tree);

  {
    std::ifstream ifs(fname);
    auto start = std::chrono::high_resolution_clock::now();
    long x, y;
    while (ifs >> x && ifs >> y) {
      op.insert(x, y);
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "took " << duration.count() << " ms to insert" << std::endl;
  }

  {
    auto start = std::chrono::high_resolution_clock::now();
    std::ofstream ofs(out_fname);
    k2tree.write_to_ostream(ofs);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "took " << duration.count() << " ms to serialize" << std::endl;
  }
}