//
// Created by cristobal on 20-12-22.
//

#include <random>
#include <unordered_map>

#include "fisher_yates.hpp"


namespace k2cache{
std::vector<unsigned long> fisher_yates(unsigned long result_size,
                                        unsigned long choice_set_size) {
  std::vector<unsigned long> result;
  std::unordered_map<unsigned long, unsigned long> state;

  for (unsigned long i = 0; i < result_size; i++) {
    unsigned long random_number = (std::rand() % (choice_set_size - i)) + i;
    unsigned long which_rand = random_number;
    unsigned long which_i = i;
    if (state.find(random_number) != state.end())
      which_rand = state[random_number];

    if (state.find(i) != state.end())
      which_i = state[i];

    state[i] = which_rand;
    state[random_number] = which_i;
  }

  for (unsigned long i = 0; i < result_size; i++)
    result.push_back(state[i] + 1);

  return result;
}
}
