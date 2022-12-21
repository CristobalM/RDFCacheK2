#ifndef _CACHE_TEST_UTIL_HPP_
#define _CACHE_TEST_UTIL_HPP_

#include <string>
#include <vector>
#include <memory>

#include "CacheContainer.hpp"
#include "k2tree/RDFTriple.hpp"
#include "manager/PredicatesCacheManager.hpp"
#include "mock_structures/FHMock.hpp"
#include "nodeids/TripleNodeId.hpp"
#include "streaming/I_TRMatchingStreamer.hpp"
#include "updating/UpdatesLoggerFilesManager.hpp"
#include <triple_external_sort.hpp>

namespace k2cache {

struct NIMDataHolders {
  std::shared_ptr<std::string> plain;
  std::shared_ptr<std::string> mapped;
  std::shared_ptr<std::string> logs;
  std::shared_ptr<std::string> logs_counter;
  NIMDataHolders();
};

struct PCMDataHolders {
  std::shared_ptr<std::string> data;
  std::shared_ptr<std::string> offsets;
  std::shared_ptr<std::string> metadata;
  PCMDataHolders();
};

struct DataHolders {
  NIMDataHolders nim_h;
  PCMDataHolders pcm_h;
};

void build_cache_test_file(const std::string &fname,
                           std::vector<TripleValue> &data);
void build_cache_test_file(const std::string &fname,
                           std::vector<TripleValue> &&data);
void build_cache_test_file(const std::string &fname);
std::vector<TripleValue> build_initial_values_triples_vector(uint64_t size);

UpdatesLoggerFilesManager mock_fh_manager(PCMDataHolders &h);
std::unique_ptr<PredicatesCacheManager> basic_pcm(DataHolders &h);

std::unique_ptr<NodeIdsManager> mock_nis(NIMDataHolders &holders);
std::unique_ptr<FHMock> mock_fh(std::shared_ptr<std::string> data);
std::unique_ptr<CacheContainer> mock_cache_container(DataHolders &holders,
                                                     bool sort_results);
std::vector<TripleNodeId> read_all_from_streamer(I_TRMatchingStreamer &streamer,
                                                 long predicate_id);

struct CreatedPredData {
  std::shared_ptr<std::string> raw_str;
  std::unique_ptr<PredicatesIndexCacheMD> get_picmd();
  explicit CreatedPredData(std::shared_ptr<std::string> raw_str);
};

CreatedPredData build_pred_data_sz(K2TreeConfig config,
                                   unsigned long predicate_id,
                                   unsigned long sz);

struct PairStream {
  virtual ~PairStream() = default;
  virtual bool has_next() = 0;
  virtual std::pair<long, long> next() = 0;
};

struct TD_Nis {
  std::unique_ptr<NIMDataHolders> nim_dh;
  std::unique_ptr<NodeIdsManager> nim;
};

struct TDWrapper {
  std::shared_ptr<std::string> tdata;
  std::unique_ptr<CacheContainer> cache_container;
  TD_Nis nis_bp;
};
CreatedPredData build_pred_data(K2TreeConfig config, unsigned long predicate_id,
                                PairStream &pair_stream);

std::stringstream
build_node_ids_seq_mem(const std::vector<unsigned long> &nis_seq);

std::shared_ptr<std::string> zero_data_str_content();

std::unique_ptr<NIMDataHolders> no_logs_static_ni_dh(std::shared_ptr<std::string> plain_ni);

TD_Nis boilerplate_nis_from_vec(const std::vector<unsigned long> &data_vec);


std::stringstream
build_k2tree_to_ss(const std::vector<TripleValue> &data);

std::unique_ptr<TDWrapper>
mock_cache_container(const std::vector<TripleValue> &triples,
                     const std::vector<unsigned long> &nids, bool sort_results);

std::shared_ptr<std::string> make_empty_ptr_shared_str();

} // namespace k2cache
#endif /* _CACHE_TEST_UTIL_HPP_ */
