#include "GroupedTriples.hpp"

GroupedTriples::GroupedTriples(
    std::unordered_map<std::string, std::vector<Triple>> &&one_var_groups,
    std::unordered_map<KeyPairStr, std::vector<Triple>, hash_pair>
        &&two_var_groups)
    : one_var_groups(std::move(one_var_groups)),
      two_var_groups(std::move(two_var_groups)) {}
