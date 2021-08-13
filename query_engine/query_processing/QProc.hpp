//
// Created by cristobal on 7/21/21.
//

#ifndef RDFCACHEK2_QPROC_HPP
#define RDFCACHEK2_QPROC_HPP

#include "VarBindingQProc.hpp"
#include "VarIndexManager.hpp"
#include "iterators/QueryIterator.hpp"
#include <PredicatesCacheManager.hpp>
#include <sparql_tree.pb.h>
class QProc {
public:
  virtual std::shared_ptr<QueryIterator>
  process_node(const proto_msg::SparqlNode &node,
               std::shared_ptr<VarBindingQProc> var_binding_qproc) = 0;
  virtual VarIndexManager &get_vim() = 0;

  virtual ~QProc() = default;
  virtual std::shared_ptr<PredicatesCacheManager> get_cache_manager() = 0;
  virtual std::shared_ptr<VarIndexManager> get_vim_ptr() = 0;
  virtual std::shared_ptr<NaiveDynamicStringDictionary>
  get_extra_str_dict_ptr() = 0;
  virtual const std::string &get_temp_files_dir() = 0;
};

#endif // RDFCACHEK2_QPROC_HPP
