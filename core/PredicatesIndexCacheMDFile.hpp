#ifndef _PREDICATES_INDEX_CACHE_MD_FILE_HPP_
#define _PREDICATES_INDEX_CACHE_MD_FILE_HPP_

#include "PredicatesIndexCacheMD.hpp"
#include "K2TreeMixed.hpp"

class PredicatesIndexCacheMDFile : private PredicatesIndexCacheMD{

  std::string fname;

  public:

  PredicatesIndexCacheMDFile(
    const std::string &fname, 
    K2TreeConfig k2tree_config);

  PredicatesIndexCacheMDFile(PredicatesIndexCacheMDFile &&other);


  bool load_single_predicate(uint64_t predicate_index);
  K2TreeMixed & fetch_k2tree(uint64_t predicate_index);

  bool has_predicate(uint64_t predicate_index);
  bool has_predicate_active(uint64_t predicate_index);
  bool has_predicate_stored(uint64_t predicate_index);
  void add_predicate(uint64_t predicate_index);
  void insert_point(uint64_t subject_index, uint64_t predicate_index, uint64_t object_index);

  void sync_file();
  void discard_in_memory_predicate(uint64_t predicate_index);

};

#endif /* _PREDICATES_INDEX_CACHE_MD_FILE_HPP_  */
