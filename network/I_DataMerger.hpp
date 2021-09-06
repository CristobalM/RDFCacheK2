//
// Created by cristobal on 06-09-21.
//

#ifndef RDFCACHEK2_I_DATAMERGER_HPP
#define RDFCACHEK2_I_DATAMERGER_HPP

class I_DataMerger {
public:
  virtual ~I_DataMerger() = default;

  virtual void
  merge_with_extra_dict(NaiveDynamicStringDictionary &input_extra_dict) = 0;
  virtual void merge_add_tree(unsigned long predicate_id,
                              K2TreeMixed &k2tree) = 0;
  virtual void merge_delete_tree(unsigned long predicate_id,
                                 K2TreeMixed &k2tree) = 0;
};

#endif // RDFCACHEK2_I_DATAMERGER_HPP
