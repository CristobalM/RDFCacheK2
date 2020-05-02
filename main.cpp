#include <iostream>
#include <chrono>

#include "block/Block.hpp"

#include "MemoryManager.hpp"

using namespace std;

int main() {
  {
    int treedepth = 9;
    int side = 1 << treedepth;
    Block block(treedepth);
    auto start = chrono::steady_clock::now();
    for (int col = 0; col < side; col++) {
      for (int row = 0; row < side; row++) {
        block.insert(col, row);
      }
    }
    auto end = chrono::steady_clock::now();

    cout << "finished inserting" << endl;

    cout << "Elapsed time milliseconds : "
         << chrono::duration_cast<chrono::milliseconds>(end - start).count()
         << endl;

    cout << "memory usage before cleanup:" << endl;
    cout << MemoryManager::instance().memory_usage() << endl;
  }

  cout << "memory usage after cleanup:" << endl;
  cout << MemoryManager::instance().memory_usage() << endl;

  int catch_me;
  std::cin >> catch_me;

  return 0;

}