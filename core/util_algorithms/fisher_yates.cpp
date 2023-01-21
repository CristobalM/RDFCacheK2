//
// Created by cristobal on 20-12-22.
//

#include <random>
#include <unordered_map>
#include <cstdint>

#include "fisher_yates.hpp"


namespace k2cache{
std::vector<uint64_t> fisher_yates(uint64_t result_size,
                                        uint64_t choice_set_size) {
  std::vector<uint64_t> result;
  std::unordered_map<uint64_t, uint64_t> state;

  for (uint64_t i = 0; i < result_size; i++) {
    uint64_t random_number = (std::rand() % (choice_set_size - i)) + i;
    uint64_t which_rand = random_number;
    uint64_t which_i = i;
    if (state.find(random_number) != state.end())
      which_rand = state[random_number];

    if (state.find(i) != state.end())
      which_i = state[i];

    state[i] = which_rand;
    state[random_number] = which_i;
  }

  for (uint64_t i = 0; i < result_size; i++)
    result.push_back(state[i] + 1);

  return result;
}
}
