#ifndef _CACHE_TEST_UTIL_HPP_
#define _CACHE_TEST_UTIL_HPP_

#include <string>
#include <vector>

#include "CacheContainer.hpp"
#include "k2tree/RDFTriple.hpp"
#include "manager/PredicatesCacheManager.hpp"
#include "mock_structures/FHMock.hpp"
#include "nodeids/TripleNodeId.hpp"
#include "streaming/I_TRMatchingStreamer.hpp"
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

std::unique_ptr<NodeIdsManager> mock_nis();
std::unique_ptr<FHMock> mock_fh();
std::unique_ptr<CacheContainer> mock_cache_container();
std::vector<TripleNodeId> read_all_from_streamer(I_TRMatchingStreamer &streamer,
                                                 long predicate_id);

struct CreatedPredData{
  const std::string raw_str;
  std::unique_ptr<PredicatesIndexCacheMD>  get_picmd();
  explicit CreatedPredData(std::string raw_str);
};

CreatedPredData build_pred_data_sz(K2TreeConfig config, unsigned long predicate_id, unsigned long sz);


struct PairStream{
  virtual ~PairStream() =  default;
  virtual bool has_next() = 0;
  virtual std::pair<long,long> next() = 0;
};

CreatedPredData
build_pred_data(K2TreeConfig config, unsigned long predicate_id, PairStream &pair_stream);

} // namespace k2cache
#endif /* _CACHE_TEST_UTIL_HPP_ */
