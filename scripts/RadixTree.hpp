//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_RADIXTREE_HPP
#define RDFCACHEK2_RADIXTREE_HPP

#include <rax.h>
#include <string>
#include <vector>
#include <memory>


template<class DataT>
class RadixTree {
  rax *inner_rt;
  std::vector<std::unique_ptr<DataT>> data_holder;
public:

  class LookupResult {
    bool _was_found;
    DataT *_result;
  protected:
    LookupResult(bool _was_found, DataT *_result ) : _was_found(_was_found), _result(_result) {}
  public:

    bool was_found(){
      return _was_found;
    }

    DataT& result(){
      if(!was_found()){
        throw std::runtime_error("Result was not found");
      }
      return *_result;
    }
  };

  RadixTree() : inner_rt(raxNew()) {}
  ~RadixTree() { raxFree(inner_rt); }

  void insert(const std::string &key, DataT &value) {
    auto d_data = std::make_unique<DataT>(value);
    auto *held_ptr = d_data.get();
    data_holder.push_back(std::move(d_data));
    raxInsert(inner_rt,
              reinterpret_cast<const unsigned char *>(key.c_str()),
              key.size(),
              reinterpret_cast<void *>(held_ptr),
              nullptr);
  }



  LookupResult &lookup(const std::string &key) {
    void *v_data = raxFind(inner_rt,
                    reinterpret_cast<const unsigned char *>(key.c_str()),
                    key.size());

    if(v_data == raxNotFound){
      return LookupResult(false, nullptr);
    }

    return LookupResult(true, reinterpret_cast<DataT *>(v_data));
  }

  void remove(const std::string &key) {
    raxRemove(inner_rt,
              reinterpret_cast<const unsigned char *>(key.c_str()),
              key.size(),
              nullptr);
  }

};


#endif //RDFCACHEK2_RADIXTREE_HPP
