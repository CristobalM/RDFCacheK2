//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_SDWRAPPER_HPP
#define RDFCACHEK2_SDWRAPPER_HPP

#include "ISDManager.hpp"
#include <StringDictionary.h>
#include <fstream>
#include <memory>

class SDWrapper : public ISDManager {
  std::unique_ptr<StringDictionary> dict;

public:
  explicit SDWrapper(std::istream &dict_is);

  void save(const std::string &file_name) override;

  unsigned long last_id() override;
  uint64_t get_resource_id_from_node_id(const NodeId &node_id) override;
  NodeId get_node_id(uint64_t index) override;

private:
  std::string _pop_string_from_ucarr(unsigned char *data, size_t size);
  std::string _extract_from_sd(StringDictionary *sd, uint64_t index);
};

#endif // RDFCACHEK2_SDWRAPPER_HPP
