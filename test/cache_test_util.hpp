#ifndef _CACHE_TEST_UTIL_HPP_
#define _CACHE_TEST_UTIL_HPP_

#include <string>
#include <vector>

#include "k2tree/RDFTriple.hpp"
#include "manager/PredicatesCacheManager.hpp"
#include "mock_structures/FHMock.hpp"
#include "updating/UpdatesLoggerFilesManager.hpp"
#include <triple_external_sort.hpp>

namespace k2cache {
void build_cache_test_file(const std::string &fname,
                           std::vector<TripleValue> &data);
void build_cache_test_file(const std::string &fname,
                           std::vector<TripleValue> &&data);
void build_cache_test_file(const std::string &fname);
std::vector<TripleValue> build_initial_values_triples_vector(uint64_t size);

UpdatesLoggerFilesManager mock_fh_manager();
std::unique_ptr<PredicatesCacheManager> basic_pcm();
std::unique_ptr<PredicatesCacheManager> basic_pcm(
    std::unique_ptr<NodeIdsManager> &&nis
    );

std::unique_ptr<NodeIdsManager> mock_nis();
std::unique_ptr<FHMock> mock_fh();
} // namespace k2cache
#endif /* _CACHE_TEST_UTIL_HPP_ */
