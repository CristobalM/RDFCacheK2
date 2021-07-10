
//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_BGPPROCESSOR_HPP
#define RDFCACHEK2_BGPPROCESSOR_HPP

#include <memory>

#include <request_msg.pb.h>

#include "PredicatesCacheManager.hpp"
#include "ResultTable.hpp"
#include "Triple.hpp"
#include "VarIndexManager.hpp"

class BGPProcessor {

  const proto_msg::BGPNode &bgp_node;
  const PredicatesCacheManager &cm;
  VarIndexManager &vim;

  std::vector<Triple> triples;

  std::shared_ptr<ResultTable> current_table;

  enum BAND_TYPE { COL_BAND = 0, ROW_BAND = 1 };

public:
  BGPProcessor(const proto_msg::BGPNode &bgp_node,
               const PredicatesCacheManager &cm, VarIndexManager &vim);

  std::shared_ptr<ResultTable> execute();

private:
  void set_triples_from_proto();
  void fill_table_with_first_triple();
  void combine_triple_with_table(const Triple &triple);
  void add_full_predicate_to_table(const Triple &triple);
  void add_column_to_table(const Triple &triple);
  void add_row_to_table(const Triple &triple);
  void combine_full_predicate_to_table(const Triple &triple);
  void combine_column_to_table(const Triple &triple);
  void combine_row_to_table(const Triple &triple);

  int find_var_position_in_headers(unsigned long var_index);

  void cross_product_table_with_triple(const Triple &triple);
  void left_join_table_with_triple_subject(const Triple &triple,
                                           unsigned long subject_var_index);
  void left_join_table_with_triple_object(const Triple &triple,
                                          unsigned long object_var_index);
  void intersect_table_with_predicate(const Triple &triple,
                                      unsigned long subject_var_index,
                                      unsigned long object_var_index);

  void cross_product_table_with_band(const Triple &triple, BAND_TYPE band_type);
  void intersect_table_with_band(unsigned long pos_in_headers,
                                 const Triple &triple, BAND_TYPE band_type);

    void add_row_to_table_if_found(const Triple &triple);
  void combine_with_no_var_triple(const Triple &triple);
};

#endif
