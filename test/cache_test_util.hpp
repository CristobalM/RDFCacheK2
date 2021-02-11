#ifndef _CACHE_TEST_UTIL_HPP_
#define _CACHE_TEST_UTIL_HPP_

#include <string>
#include <vector>

#include <external_sort.hpp>

void build_cache_test_file(const std::string &fname,
                           std::vector<TripleValue> &data);
void build_cache_test_file(const std::string &fname,
                           std::vector<TripleValue> &&data);
std::vector<TripleValue> build_initial_values_triples_vector(uint64_t size);

#endif /* _CACHE_TEST_UTIL_HPP_ */
