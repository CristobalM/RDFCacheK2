
#include <cassert>
#include <sstream>

#include "NaiveDynamicStringDictionary.hpp"
#include "serialization_util.hpp"

NaiveDynamicStringDictionary::NaiveDynamicStringDictionary(
    std::vector<RDFResource> &&resources_extra,
    std::unordered_map<RDFResource, unsigned long, NaiveHash>
        &&reverse_resources)
    : resources_extra(std::move(resources_extra)),
      reverse_resources(std::move(reverse_resources)) {}

void NaiveDynamicStringDictionary::serialize_dict(
    std::vector<RDFResource> &resources, const std::string &fname) {
  std::ofstream ofs(fname, std::ios::out | std::ofstream::binary);
  uint64_t size = 0;
  for (auto &resource : resources) {
    size += resource.value.size() + 1 + sizeof(uint16_t);
  }

  write_u64(ofs, size);

  for (auto &resource : resources) {
    write_u16(ofs, resource.resource_type);
    auto &s = resource.value;
    ofs.write(s.data(), s.size() + 1);
  }
}

std::vector<RDFResource>
NaiveDynamicStringDictionary::deserialize_dict(const std::string &fname) {
  std::ifstream ifs(fname, std::ios::in | std::ifstream::binary);
  uint64_t size = read_u64(ifs);
  std::vector<char> buffer(size, 0);
  ifs.read(buffer.data(), size);
  std::vector<RDFResource> out;
  uint64_t offset_base = 0;
  for (uint64_t i = 0; i < buffer.size(); i++) {
    if (!buffer[i]) {
      std::string raw_type(buffer.data(), buffer.data() + sizeof(uint16_t));
      std::stringstream ss(raw_type);
      auto type = static_cast<RDFResourceType>(read_u16(ss));
      std::string s(buffer.data() + sizeof(uint16_t) + offset_base);
      out.push_back(RDFResource(std::move(s), type));
      offset_base = i + 1;
    }
  }
  return out;
}

std::unordered_map<RDFResource, unsigned long, NaiveHash>
NaiveDynamicStringDictionary::create_reverse(std::vector<RDFResource> &input) {
  std::unordered_map<RDFResource, unsigned long, NaiveHash> out;
  unsigned long counter = 0;
  for (auto &resource : input) {
    out[resource] = counter++;
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
  return NaiveDynamicStringDictionary(std::move(res), std::move(reverse_res));
}

void NaiveDynamicStringDictionary::add_resource(RDFResource resource) {
  reverse_resources[resource] = resources_extra.size();
  resources_extra.push_back(std::move(resource));
}

unsigned long NaiveDynamicStringDictionary::locate_resource(
    const RDFResource &resource) const {
  if (reverse_resources.find(resource) == reverse_resources.end())
    return 0;
  return reverse_resources.at(resource) + 1;
}

RDFResource
NaiveDynamicStringDictionary::extract_resource(unsigned long id) const {
  long next_id = static_cast<long>(id) - 1;
  assert(id > 0 && next_id >= 0 && next_id < (long)resources_extra.size());
  return resources_extra[next_id];
}

size_t NaiveDynamicStringDictionary::size() const {
  return resources_extra.size();
}
