//
// Created by Cristobal Miranda, 2020
//

#include <gtest/gtest.h>
#include <memory>
#include <filesystem>

#include <EmptyISDManager.hpp>
#include <PredicatesCacheManager.hpp>
#include <RDFTriple.hpp>
#include <external_sort.hpp>
#include <serialization_util.hpp>
#include <PredicatesIndexFileBuilder.hpp>
#include <K2TreeMixed.hpp>

static void build_test_file(const std::string &fname, std::vector<TripleValue> &data){
  std::string plain_predicates_fname = fname + "plain_predicates_file.bin";
  {
    std::ofstream ofs_plain(plain_predicates_fname, std::ios::out | std::ios::binary | std::ios::trunc);

    write_u64(ofs_plain, data.size());
    for(auto &tvalue: data){
      tvalue.write_to_file(ofs_plain);
    }
  }

  K2TreeConfig config;
  config.cut_depth = 10;
  config.max_node_count = 256;
  config.treedepth = 32;


  std::ifstream ifs_plain(plain_predicates_fname, std::ios::in | std::ios::binary);
  
  std::string index_fname_tmp = fname  +  ".tmp";


  {
  std::ofstream ofs_index(fname, std::ios::out | std::ios::binary | std::ios::trunc);
  std::fstream ofs_index_tmp(index_fname_tmp, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);

  PredicatesIndexFileBuilder::build(ifs_plain, ofs_index, ofs_index_tmp, config);
  }

  std::filesystem::remove(plain_predicates_fname);
  std::filesystem::remove(index_fname_tmp);

}

TEST(predicates_cache_manager_test, test1) {
  K2TreeConfig config;
  config.treedepth = 32;
  config.max_node_count = 256;
  config.cut_depth = 10;
  
  std::string fname = "predicates.bin";

  std::vector<TripleValue> data;
  uint64_t sz =  10000;
  for(uint64_t i = 1; i <= sz; i++){
    data.push_back(TripleValue(i,i,i));
  }
  build_test_file(fname, data);
  PredicatesCacheManager pcm(std::make_unique<EmptyISDManager>(), config, fname);

  for(uint64_t i = 1; i <= sz; i++){
    // ASSERT_TRUE(pcm.has_triple(i, i, i));
    ASSERT_TRUE(pcm.get_predicates_index_cache().fetch_k2tree(i).has(i, i));
  }

  auto subject = RDFResource("subject1", RDF_TYPE_IRI);
  auto predicate = RDFResource("predicate1", RDF_TYPE_IRI);
  auto object = RDFResource("object1", RDF_TYPE_LITERAL);


  auto resource =  RDFTripleResource(subject,
                                   predicate,
                                   object);
  pcm.add_triple(resource);

  auto subject_index = pcm.get_resource_index(subject);
  auto predicate_index = pcm.get_resource_index(predicate);
  auto object_index = pcm.get_resource_index(object);

  ASSERT_TRUE(pcm.has_triple(resource));
  ASSERT_TRUE(pcm.get_predicates_index_cache().fetch_k2tree(predicate_index).has(subject_index, object_index));

  std::filesystem::remove(fname);
}
