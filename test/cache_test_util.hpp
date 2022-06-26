#ifndef _CACHE_TEST_UTIL_HPP_
#define _CACHE_TEST_UTIL_HPP_

#include <string>
#include <vector>

#include "k2tree/RDFTriple.hpp"
#include <Cache.hpp>
#include <triple_external_sort.hpp>

void build_cache_test_file(const std::string &fname,
                           std::vector<TripleValue> &data);
void build_cache_test_file(const std::string &fname,
                           std::vector<TripleValue> &&data);
void build_cache_test_file(const std::string &fname);
std::vector<TripleValue> build_initial_values_triples_vector(uint64_t size);

UpdatesLoggerFilesManager mock_fh_manager();
PredicatesCacheManager basic_pcm();

#endif /* _CACHE_TEST_UTIL_HPP_ */
