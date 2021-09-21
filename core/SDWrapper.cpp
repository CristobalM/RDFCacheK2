//
// Created by cristobal on 9/9/21.
//

#include "SDWrapper.hpp"

SDWrapper::SDWrapper(std::istream &dict_is)
    : dict(std::unique_ptr<StringDictionary>(
          StringDictionary::load(dict_is, 0))) {}

void SDWrapper::save(const std::string &file_name) {
  std::ofstream ofs(file_name, std::ofstream::binary);
  dict->save(ofs);
}

unsigned long SDWrapper::last_id() { return dict->numElements(); }
uint64_t SDWrapper::get_resource_id_from_node_id(const NodeId &node_id) {
  return dict->locate(reinterpret_cast<unsigned char *>(
                          const_cast<uint8_t *>(node_id.get_raw())),
                      (unsigned int)NodeId::DATA_SIZE);
}

NodeId SDWrapper::get_node_id(uint64_t index) {
  return NodeId(_extract_from_sd(dict.get(), index));
}

std::string SDWrapper::_extract_from_sd(StringDictionary *sd, uint64_t index) {
  unsigned int string_size;
  unsigned char *data = sd->extract(index, &string_size);
  return _pop_string_from_ucarr(data, string_size);
}

std::string SDWrapper::_pop_string_from_ucarr(unsigned char *data,
                                              size_t size) {
  std::string result(reinterpret_cast<char *>(data), size);
  delete[] data;
  return result;
}
