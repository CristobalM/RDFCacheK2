#include <chrono>
#include <iostream>

#include "K2Tree.hpp"

#include "MemoryManager.hpp"

using namespace std;

int main() {
  {
    int treedepth = 9;
    int side = 1 << treedepth;
    K2Tree k2tree(treedepth);
    auto start = chrono::steady_clock::now();
    for (int col = 0; col < side; col++) {
      for (int row = 0; row < side; row++) {
        k2tree.insert(col, row);
      }
    }
    auto end = chrono::steady_clock::now();

    cout << "finished inserting" << endl;

    cout << "Elapsed time milliseconds : "
         << chrono::duration_cast<chrono::milliseconds>(end - start).count()
         << endl;

    start = chrono::steady_clock::now();
    auto scanned_points = k2tree.scan_points();
    end = chrono::steady_clock::now();

    cout << "scanned points amount: " << scanned_points.size() << endl;

    cout << "first ten scanned: " << endl;

    for (int i = 0; i < 10; i++) {
      cout << scanned_points[i].first << ", " << scanned_points[i].second
           << endl;
    }

    cout << "Elapsed time milliseconds scanning : "
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