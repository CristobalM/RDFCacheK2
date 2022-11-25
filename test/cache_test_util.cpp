
#include "cache_test_util.hpp"
#include "mock_structures/FHMock.hpp"

#include <filesystem>
#include <iostream>

#include "CacheContainerImpl.hpp"
#include "builder/PredicatesIndexFileBuilder.hpp"
#include "fic/NoFIC.hpp"
#include "k2tree/K2TreeMixed.hpp"
#include "manager/PCMFactory.hpp"
#include "manager/PredicatesCacheManagerImpl.hpp"
#include "nodeids/NodeIdsManagerFactory.hpp"
#include "nodeids/NodeIdsManagerIdentity.hpp"
#include "replacement/NoCachingReplacement.hpp"
#include "updating/NoUpdate.hpp"
#include <serialization_util.hpp>

namespace k2cache {

void build_cache_test_file(const std::string &fname,
                           std::vector<TripleValue> &data) {
  std::string plain_predicates_fname = fname + "plain_predicates_file.bin";
  {
    std::ofstream ofs_plain(plain_predicates_fname,
                            std::ios::out | std::ios::binary | std::ios::trunc);

    write_u64(ofs_plain, data.size());
    for (auto &tvalue : data) {
      tvalue.write_to_file(ofs_plain);
    }
  }

  K2TreeConfig config;
  config.cut_depth = 10;
  config.max_node_count = 256;
  config.treedepth = 32;

  std::ifstream ifs_plain(plain_predicates_fname,
                          std::ios::in | std::ios::binary);

  std::string index_fname_tmp = fname + ".tmp";

  {
    std::ofstream ofs_index(fname,
                            std::ios::out | std::ios::binary | std::ios::trunc);
    std::fstream ofs_index_tmp(index_fname_tmp, std::ios::in | std::ios::out |
                                                    std::ios::binary |
                                                    std::ios::trunc);

    PredicatesIndexFileBuilder::build(ifs_plain, ofs_index, ofs_index_tmp,
                                      config);
  }

  std::filesystem::remove(plain_predicates_fname);
  std::filesystem::remove(index_fname_tmp);
}

void build_cache_test_file(const std::string &fname,
                           std::vector<TripleValue> &&data) {
  build_cache_test_file(fname, data);
}

std::vector<TripleValue> build_initial_values_triples_vector(uint64_t size) {
  std::vector<TripleValue> result;

  for (uint64_t i = 1; i <= size; i++) {
    result.emplace_back(i, i, i);
  }

  return result;
}

void build_cache_test_file(const std::string &fname) {
  build_cache_test_file(fname, {});
}

UpdatesLoggerFilesManager mock_fh_manager(PCMDataHolders &h) {
  auto fh = std::make_unique<FHMock>(h.data);
  auto fh_offsets = std::make_unique<FHMock>(h.offsets);
  auto fh_metadata = std::make_unique<FHMock>(h.metadata);
  return {std::move(fh), std::move(fh_offsets), std::move(fh_metadata)};
}

std::unique_ptr<PredicatesCacheManager> basic_pcm(DataHolders &h) {
  K2TreeConfig config{};
  config.treedepth = 32;
  config.cut_depth = 10;
  config.max_node_count = 256;

  std::unique_ptr<I_FileRWHandler> fh_pcm{};
  {
    auto *plain_ptr = &h.nim_h.plain;
    (void)plain_ptr;
    fh_pcm = mock_fh(h.nim_h.plain);
    auto fh_writer = fh_pcm->get_writer(std::ios::out | std::ios::binary);
    PredicatesCacheMetadata metadata_pcm(config);
    metadata_pcm.write_to_ostream(fh_writer->get_ostream());
    fh_writer->flush();
  }
  auto *plain_ptr = &h.nim_h.plain;
  (void)plain_ptr;
  auto pcm =
      PCMFactory::create(std::move(fh_pcm), mock_fh_manager(h.pcm_h), false);
  return pcm;
}
std::unique_ptr<NodeIdsManager> mock_nis(NIMDataHolders &holders) {
  auto plain_fh = mock_fh(holders.plain);
  auto mapped_fh = mock_fh(holders.mapped);
  auto logs_fh = mock_fh(holders.logs);
  auto logs_counter_fh = mock_fh(holders.logs_counter);
  return NodeIdsManagerFactory::create(std::move(plain_fh),
                                       std::move(mapped_fh), std::move(logs_fh),
                                       std::move(logs_counter_fh));
}
std::unique_ptr<FHMock> mock_fh(std::string &data) {
  return std::make_unique<FHMock>(data);
}

std::unique_ptr<CacheContainer>
mock_cache_container(DataHolders &holders, bool sort_results = false) {
  return std::make_unique<CacheContainerImpl>(
      basic_pcm(holders), mock_nis(holders.nim_h),
      std::make_unique<NoCachingReplacement>(),
      I_CacheReplacement::REPLACEMENT_STRATEGY::NO_CACHING, sort_results);
}
std::vector<TripleNodeId> read_all_from_streamer(I_TRMatchingStreamer &streamer,
                                                 long predicate_id) {
  std::vector<TripleNodeId> result;
  auto response = streamer.get_next_response();
  auto &msg = response.stream_of_triples_matching_pattern_response();
  for (auto i = 0; i < msg.matching_values_size(); i++) {
    auto &curr = msg.matching_values(i);
    if (curr.single_match_size() != 2) {
      throw std::runtime_error("unexpected size, expecting 2, but was = " +
                               std::to_string(curr.single_match_size()));
    }

    result.emplace_back(NodeId((long)curr.single_match(0).encoded_data()),
                        NodeId(predicate_id),
                        NodeId((long)curr.single_match(1).encoded_data()));
  }
  return result;
}

CreatedPredData build_pred_data_sz(K2TreeConfig config,
                                   unsigned long predicate_id,
                                   unsigned long sz) {
  struct my_pair_str : public PairStream {
    unsigned long sz = 0;
    unsigned long pred = 0;
    unsigned long i = 1;
    unsigned long j = 1;
    bool has_next() override { return i <= sz && j < 10; }
    std::pair<long, long> next() override {
      auto out = std::make_pair(i, j);
      j++;
      if (j == 10) {
        i++;
        j = i;
      }
      return out;
    }
  };

  my_pair_str mps;
  mps.sz = sz;
  mps.pred = predicate_id;
  return build_pred_data(config, predicate_id, mps);
}

CreatedPredData build_pred_data(K2TreeConfig config, unsigned long predicate_id,
                                PairStream &pair_stream) {
  std::stringstream ss;
  auto out = std::make_unique<std::stringstream>();
  std::stringstream tmp;
  write_u64(ss, 0);
  auto sz = 0L;
  while (pair_stream.has_next()) {
    const auto &p = pair_stream.next();
    TripleValue(p.first, predicate_id, p.second).write_to_file(ss);
    sz++;
  }
  ss.seekg(0);
  auto curr = ss.tellp();
  ss.seekp(0);
  write_u64(ss, sz);
  ss.seekp(curr);

  out->seekp(0);

  PredicatesIndexFileBuilder::build(ss, *out, tmp, config);

  CreatedPredData cpd(out->str());
  return cpd;
}

std::unique_ptr<PredicatesIndexCacheMD> CreatedPredData::get_picmd() {
  auto frw_handler = std::make_unique<FHMock>(raw_str);
  return std::make_unique<PredicatesIndexCacheMD>(std::move(frw_handler));
}
CreatedPredData::CreatedPredData(std::string raw_str)
    : raw_str(std::move(raw_str)) {}

std::stringstream
build_node_ids_seq_mem(const std::vector<unsigned long> &nis_seq) {
  std::stringstream ss;
  write_u64(ss, nis_seq.size());
  for (auto v : nis_seq) {
    write_u64(ss, v);
  }
  return ss;
}

std::string zero_data_str_content() {
  std::stringstream ss;
  write_u64(ss, 0);
  return ss.str();
}

std::unique_ptr<NIMDataHolders> no_logs_static_ni_dh(std::string plain_ni) {
  auto dh = std::make_unique<NIMDataHolders>();
  dh->plain = std::move(plain_ni);
  dh->logs = zero_data_str_content();
  dh->logs_counter = zero_data_str_content();
  dh->mapped = zero_data_str_content();
  return dh;
}

TD_Nis boilerplate_nis_from_vec(const std::vector<unsigned long> &data_vec) {
  auto ss = build_node_ids_seq_mem(data_vec);
  auto dh = no_logs_static_ni_dh(ss.str());
  auto nis = mock_nis(*dh);
  TD_Nis out;
  out.nim_dh = std::move(dh);
  out.nim = std::move(nis);
  return out;
}

std::stringstream build_k2tree_to_ss(const std::vector<TripleValue> &data) {
  std::stringstream plain_ss;
  write_u64(plain_ss, data.size());
  for (auto t : data) {
    t.write_to_file(plain_ss);
  }
  return plain_ss;
}

std::unique_ptr<TDWrapper>
mock_cache_container(const std::vector<TripleValue> &triples,
                     const std::vector<unsigned long> &nids,
                     bool sort_results = false) {
  auto plain_ss = build_k2tree_to_ss(triples);
  auto td_wrapper = std::make_unique<TDWrapper>();
  td_wrapper->tdata = plain_ss.str();
  auto fhmock = std::make_unique<FHMock>(td_wrapper->tdata);
  auto pcimd = std::make_unique<PredicatesIndexCacheMD>(std::move(fhmock));
  auto updl = std::make_unique<NoUpdate>();
  auto nofic = std::make_unique<NoFIC>();
  auto pcm = std::make_unique<PredicatesCacheManagerImpl>(
      std::move(pcimd), std::move(updl), std::move(nofic));

  auto bp = boilerplate_nis_from_vec(nids);
  auto norep = std::make_unique<NoCachingReplacement>();
  auto cache_container = std::make_unique<CacheContainerImpl>(
      std::move(pcm), std::move(bp.nim), std::move(norep),
      I_CacheReplacement::NO_CACHING, sort_results);

  td_wrapper->cache_container = std::move(cache_container);
  td_wrapper->nis_bp = std::move(bp);

  return td_wrapper;
}

} // namespace k2cache
