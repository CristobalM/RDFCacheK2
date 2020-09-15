
#include <cassert>

#include "NaiveDynamicStringDictionary.hpp"
#include "serialization_util.hpp"

NaiveDynamicStringDictionary::NaiveDynamicStringDictionary(
    std::vector<std::string> &&iris_extra,
    std::vector<std::string> &&blanks_extra,
    std::vector<std::string> &&literals_extra,
    std::unordered_map<std::string, unsigned long> &&reverse_iris_extra,
    std::unordered_map<std::string, unsigned long> &&reverse_blanks_extra,
    std::unordered_map<std::string, unsigned long> &&reverse_literals_extra)
    : iris_extra(iris_extra), blanks_extra(blanks_extra),
      literals_extra(literals_extra), reverse_iris_extra(reverse_iris_extra),
      reverse_blanks_extra(reverse_blanks_extra),
      reverse_literals_extra(reverse_literals_extra) {}

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

void NaiveDynamicStringDictionary::save(const std::string &iris_fname,
                                        const std::string &blanks_fname,
                                        const std::string &literals_fname) {
  serialize_dict(iris_extra, iris_fname);
  serialize_dict(blanks_extra, blanks_fname);
  serialize_dict(literals_extra, literals_fname);
}

NaiveDynamicStringDictionary
NaiveDynamicStringDictionary::load(const std::string &iris_fname,
                                   const std::string &blanks_fname,
                                   const std::string &literals_fname) {
  auto iris = deserialize_dict(iris_fname);
  auto reverse_iris = create_reverse(iris);
  auto blanks = deserialize_dict(blanks_fname);
  auto reverse_blanks = create_reverse(blanks);
  auto literals = deserialize_dict(literals_fname);
  auto reverse_literals = create_reverse(literals);
  return NaiveDynamicStringDictionary(
      std::move(iris), std::move(blanks), std::move(literals),
      std::move(reverse_iris), std::move(reverse_blanks),
      std::move(reverse_literals));
}

void NaiveDynamicStringDictionary::add_iri(std::string iri) {
  reverse_iris_extra[iri] = iris_extra.size();
  iris_extra.push_back(std::move(iri));
}

void NaiveDynamicStringDictionary::add_blank(std::string blank) {
  reverse_blanks_extra[blank] = blanks_extra.size();
  blanks_extra.push_back(std::move(blank));
}

void NaiveDynamicStringDictionary::add_literal(std::string literal) {
  reverse_literals_extra[literal] = literals_extra.size();
  literals_extra.push_back(std::move(literal));
}

unsigned long
NaiveDynamicStringDictionary::locate_iri(const std::string &iri) const {
  if (reverse_iris_extra.find(iri) == reverse_iris_extra.end())
    return 0;
  return reverse_iris_extra.at(iri) + 1;
}

unsigned long
NaiveDynamicStringDictionary::locate_blank(const std::string &blank) const {
  if (reverse_blanks_extra.find(blank) == reverse_blanks_extra.end())
    return 0;
  return reverse_blanks_extra.at(blank) + 1 + reverse_iris_extra.size();
}

unsigned long
NaiveDynamicStringDictionary::locate_literal(const std::string &literal) const {
  if (reverse_literals_extra.find(literal) == reverse_literals_extra.end())
    return 0;
  return reverse_literals_extra.at(literal) + reverse_blanks_extra.size() + 1 +
         reverse_iris_extra.size();
}

std::string NaiveDynamicStringDictionary::extract_iri(unsigned long id) const {
  auto next_id = id - 1;
  assert(id > 0 && next_id >= 0 && next_id < iris_extra.size());
  return iris_extra[next_id];
}

std::string
NaiveDynamicStringDictionary::extract_blank(unsigned long id) const {
  auto next_id = id - 1 - iris_extra.size();
  assert(id >= iris_extra.size() + 1 && next_id >= 0 &&
         next_id < blanks_extra.size());
  return blanks_extra[next_id];
}

std::string
NaiveDynamicStringDictionary::extract_literal(unsigned long id) const {
  auto next_id = id - 1 - iris_extra.size() - blanks_extra.size();
  assert(id >= iris_extra.size() + blanks_extra.size() + 1 && next_id >= 0 &&
         next_id < literals_extra.size());
  return literals_extra[next_id];
}

unsigned long NaiveDynamicStringDictionary::locate_resource(
    const RDFResource &resource) const {
  switch (resource.resource_type) {
  case RDF_TYPE_IRI:
    return locate_iri(resource.value);
  case RDF_TYPE_BLANK:
    return locate_blank(resource.value);
  case RDF_TYPE_LITERAL:
    return locate_literal(resource.value);
  default:
    return 0;
  }
}

RDFResource
NaiveDynamicStringDictionary::extract_resource(unsigned long id) const {
  assert(id > 0);
  std::string value;
  RDFResourceType res_type;

  if (id <= iris_extra.size()) {
    value = extract_iri(id);
    res_type = RDF_TYPE_IRI;
  } else if (id <= iris_extra.size() + blanks_extra.size()) {
    value = extract_blank(id);
    res_type = RDF_TYPE_BLANK;
  } else {
    value = extract_literal(id);
    res_type = RDF_TYPE_LITERAL;
  }

  return RDFResource(std::move(value), res_type);
}

void NaiveDynamicStringDictionary::add_resource(RDFResource &resource) {
  switch (resource.resource_type) {
  case RDF_TYPE_IRI:
    add_iri(resource.value);
    break;
  case RDF_TYPE_BLANK:
    add_blank(resource.value);
    break;
  case RDF_TYPE_LITERAL:
    add_literal(resource.value);
    break;
  }
}
