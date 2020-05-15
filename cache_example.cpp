//
// Created by Cristobal Miranda, 2020
//

#include "GraphResult.hpp"

#include <iostream>

using namespace std;

int main() {
  GraphResult cache_result;


  int tree_depth = 4;
  int side = 1 << tree_depth;
  for (int predicate = 0; predicate < side; predicate++) {
    cache_result.insert_predicate(predicate, side);
    for (int col = 0; col < side; col++) {
      for (int row = 0; row < side; row++) {
        cache_result.insert_triple(col, predicate, row);
      }
    }
  }

  auto serialized = cache_result.serialize_result();

  auto restored_cr = GraphResult::from_binary(serialized);

  cout << "Has triple 1, 0, 1?: " << (restored_cr->has_triple(1, 0, 1) ? "TRUE" : "FALSE") << endl;
  cout << "Has triple 1, 0, 2?: " << (restored_cr->has_triple(1, 0, 2) ? "TRUE" : "FALSE") << endl;

}