#ifndef _CACHE_TEST_UTIL_HPP_
#define _CACHE_TEST_UTIL_HPP_

#include <string>
#include <vector>

#include <Cache.hpp>
#include <RDFTriple.hpp>
#include <ResultTable.hpp>
#include <triple_external_sort.hpp>

void build_cache_test_file(const std::string &fname,
                           std::vector<TripleValue> &data);
void build_cache_test_file(const std::string &fname,
                           std::vector<TripleValue> &&data);
void build_cache_test_file(const std::string &fname);
std::vector<TripleValue> build_initial_values_triples_vector(uint64_t size);

void print_table_debug(
    ResultTable &table,
    std::unordered_map<unsigned long, std::string> &reverse_map,
    const std::vector<std::vector<RDFResource>> &translated_table);

UpdatesLoggerFilesManager mock_fh_manager();
#endif /* _CACHE_TEST_UTIL_HPP_ */
