//
// Created by Cristobal Miranda, 2020
//

#include "CacheResult.hpp"

#include <iostream>

using namespace std;

int main(){
  CacheResult cache_result;


  cache_result.insert_predicate(0, 10);
  cache_result.insert_triple(1, 0, 1);
  cache_result.insert_triple(1, 0, 2);

  auto serialized = cache_result.serialize_result();

  auto restored_cr = CacheResult::from_binary(serialized);

  cout << "Has triple 1, 0, 1?: " << (restored_cr->has_triple(1, 0, 1) ? "TRUE" : "FALSE" ) << endl;
  cout << "Has triple 1, 0, 2?: " << (restored_cr->has_triple(1, 0, 2) ? "TRUE" : "FALSE" ) << endl;

  for(int col = 0; col < 16; col++){
    for(int row = 0; row < 16; row++){
      if((col == 1 && row == 1) ||  (col == 1 && row ==2) ){
        continue;
      }
      if(restored_cr->has_triple(col, 0, row))
      cout << "ERROR: found point (" << col << ", " << 0 << ", " << row << ") which was never created" << endl;
    }
  }
}