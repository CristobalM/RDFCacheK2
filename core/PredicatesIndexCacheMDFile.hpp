#ifndef _PREDICATES_INDEX_CACHE_MD_FILE_HPP_
#define _PREDICATES_INDEX_CACHE_MD_FILE_HPP_

#include "K2TreeMixed.hpp"
#include "PredicatesIndexCacheMD.hpp"

class PredicatesIndexCacheMDFile : private PredicatesIndexCacheMD {

  std::string fname;

  using buf_t = std::array<char, 10'000'000>;

  buf_t buffer;

public:
  explicit PredicatesIndexCacheMDFile(const std::string &fname);

  PredicatesIndexCacheMDFile(PredicatesIndexCacheMDFile &&other) noexcept;

  bool load_single_predicate(uint64_t predicate_index);
  PredicateFetchResult fetch_k2tree(uint64_t predicate_index);

  bool has_predicate(uint64_t predicate_index);
  bool has_predicate_active(uint64_t predicate_index);
  bool has_predicate_stored(uint64_t predicate_index);
  void add_predicate(uint64_t predicate_index);
  void insert_point(uint64_t subject_index, uint64_t predicate_index,
                    uint64_t object_index);

  void sync_file();
  void discard_in_memory_predicate(uint64_t predicate_index);

  K2TreeConfig get_config();

  const std::vector<uint64_t> &get_predicates_ids();
  const PredicatesCacheMetadata &get_metadata();

  void load_all_predicates();

  void set_update_logger(I_UpdateLoggerPCM *input_update_logger);

private:
  static std::unique_ptr<std::istream> load_file(const std::string &fname,
                                                 buf_t &buffer);
};

#endif /* _PREDICATES_INDEX_CACHE_MD_FILE_HPP_  */
