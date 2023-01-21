//
// Created by Cristobal Miranda on 14-01-23.
//

#ifndef RDFCACHEK2_RANDOM_DATASET_GENERATION_HPP
#define RDFCACHEK2_RANDOM_DATASET_GENERATION_HPP

#include <cstdint>

#include "k2tree/K2TreeMixed.hpp"
#include "I_OStream.hpp"
#include "I_IOStream.hpp"
namespace k2cache{

void generate_random_dataset(
    K2TreeConfig config,
    uint64_t triples_num,
    uint64_t resources_num,
    I_OStream &trees_ostream,
    I_IOStream &trees_tmp_ostream,
    I_OStream &nodeids_ostream
    );
}
#endif // RDFCACHEK2_RANDOM_DATASET_GENERATION_HPP
