#include <FourSidedQueue.hpp>
#include <iostream>

int main() {
  FourSidedQueue<int> f;
  f.push_back(1, 1);
  f.push_back(2, 2);
  f.push_back(3, 3);
  f.push_back(4, 3);
  /*
  while(!f.empty()){
      std::cout << f.pop_next(3) << std::endl;
  }
  */
}