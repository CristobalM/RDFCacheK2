
#include <cassert>

#include "NaiveDynamicStringDictionary.hpp"
#include "serialization_util.hpp"

NaiveDynamicStringDictionary::NaiveDynamicStringDictionary(
    std::vector<std::string> &&resources_extra,
    std::unordered_map<std::string, unsigned long> &&reverse_resources)
    : resources_extra(resources_extra), reverse_resources(reverse_resources)
    {}

void NaiveDynamicStringDictionary::serialize_dict(
    std::vector<std::string> &strings, const std::string &fname) {
  std::ofstream ofs(fname, std::ios::out | std::ofstream::binary);
  uint64_t size = 0;
  for (auto &s : strings) {
    size += s.size() + 1;
  }

  write_u64(ofs, size);
  for (auto &s : strings) {
    ofs.write(s.data(), s.size() + 1);
  }
}

std::vector<std::string>
NaiveDynamicStringDictionary::deserialize_dict(const std::string &fname) {
  std::ifstream ifs(fname, std::ios::in | std::ifstream::binary);
  uint64_t size = read_u64(ifs);
  std::vector<char> buffer(size, 0);
  ifs.read(buffer.data(), size);
  std::vector<std::string> out;
  uint64_t offset_base = 0;
  for (uint64_t i = 0; i < buffer.size(); i++) {
    if (!buffer[i]) {
      std::string s(buffer.data() + offset_base);
      out.push_back(std::move(s));
      offset_base = i + 1;
    }
  }
  return out;
}

std::unordered_map<std::string, unsigned long>
NaiveDynamicStringDictionary::create_reverse(std::vector<std::string> &input) {
  std::unordered_map<std::string, unsigned long> out;
  unsigned long counter = 0;
  for (auto &s : input) {
    out[s] = counter++;
  }
  return out;
}

void NaiveDynamicStringDictionary::save(const std::string &res_fname) {
  serialize_dict(resources_extra, res_fname);
}

NaiveDynamicStringDictionary
NaiveDynamicStringDictionary::load(const std::string &res_fname) {
  auto res = deserialize_dict(res_fname);
  auto reverse_res = create_reverse(res);
  return NaiveDynamicStringDictionary(
      std::move(res), std::move(reverse_res));
}


void NaiveDynamicStringDictionary::add_resource(std::string resource) {
  reverse_resources[resource] = resources_extra.size();
  resources_extra.push_back(std::move(resource));
}


unsigned long
NaiveDynamicStringDictionary::locate_resource(const std::string &literal) const {
  if (reverse_resources.find(literal) == reverse_resources.end())
    return 0;
  return reverse_resources.at(literal) + 1;
}


std::string NaiveDynamicStringDictionary::extract_resource(unsigned long id) const {
  long next_id = static_cast<long>(id) - 1;
  assert(id > 0 && next_id >= 0 && next_id < (long)resources_extra.size());
  return resources_extra[next_id];
}

