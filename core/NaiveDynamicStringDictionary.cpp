
#include "NaiveDynamicStringDictionary.hpp"

NaiveDynamicStringDictionary::NaiveDynamicStringDictionary(
    std::vector<std::string> &&subjects_extra,
    std::vector<std::string> &&predicates_extra,
    std::vector<std::string> &&objects_extra,
    std::unordered_map<std::string, unsigned long> &&reverse_subjects_extra,
    std::unordered_map<std::string, unsigned long> &&reverse_predicates_extra,
    std::unordered_map<std::string, unsigned long> &&reverse_objects_extra)
    : subjects_extra(subjects_extra), predicates_extra(predicates_extra),
      objects_extra(objects_extra),
      reverse_subjects_extra(reverse_subjects_extra),
      reverse_predicates_extra(reverse_predicates_extra),
      reverse_objects_extra(reverse_objects_extra) {}

void NaiveDynamicStringDictionary::serialize_dict(
    std::vector<std::string> &strings, const std::string &fname) {
  std::ofstream ofs(fname, std::ios::out | std::ofstream::binary);
  uint64_t size = 0;
  for (auto &s : strings) {
    size += s.size() + 1;
  }

  ofs.write(reinterpret_cast<char *>(&size), sizeof(uint64_t));
  for (auto &s : strings) {
    ofs.write(s.data(), s.size() + 1);
  }
}

std::vector<std::string>
NaiveDynamicStringDictionary::deserialize_dict(const std::string &fname) {
  std::ifstream ifs(fname, std::ios::in | std::ifstream::binary);
  uint64_t size;
  ifs.read(reinterpret_cast<char *>(&size), sizeof(uint64_t));
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

void NaiveDynamicStringDictionary::save(const std::string &sub_fname,
                                        const std::string &pred_fname,
                                        const std::string &obj_fname) {
  serialize_dict(subjects_extra, sub_fname);
  serialize_dict(predicates_extra, pred_fname);
  serialize_dict(objects_extra, obj_fname);
}

NaiveDynamicStringDictionary
NaiveDynamicStringDictionary::load(const std::string &sub_fname,
                                   const std::string &pred_fname,
                                   const std::string &obj_fname) {
  auto subjects = deserialize_dict(sub_fname);
  auto reverse_subjects = create_reverse(subjects);
  auto predicates = deserialize_dict(pred_fname);
  auto reverse_predicates = create_reverse(predicates);
  auto objects = deserialize_dict(obj_fname);
  auto reverse_objects = create_reverse(objects);
  return NaiveDynamicStringDictionary(
      std::move(subjects), std::move(predicates), std::move(objects),
      std::move(reverse_subjects), std::move(reverse_predicates),
      std::move(reverse_objects));
}

void NaiveDynamicStringDictionary::add_subject(std::string subject) {
  reverse_subjects_extra[subject] = subjects_extra.size();
  subjects_extra.push_back(std::move(subject));
}

void NaiveDynamicStringDictionary::add_predicate(std::string predicate) {
  reverse_predicates_extra[predicate] = predicates_extra.size();
  predicates_extra.push_back(std::move(predicate));
}

void NaiveDynamicStringDictionary::add_object(std::string object) {
  reverse_objects_extra[object] = objects_extra.size();
  objects_extra.push_back(std::move(object));
}

unsigned long
NaiveDynamicStringDictionary::locate_subject(const std::string &subject) const {
  return reverse_subjects_extra.at(subject);
}

unsigned long NaiveDynamicStringDictionary::locate_predicate(
    const std::string &predicate) const {
  return reverse_predicates_extra.at(predicate);
}

unsigned long
NaiveDynamicStringDictionary::locate_object(const std::string &object) const {
  return reverse_objects_extra.at(object);
}

std::string
NaiveDynamicStringDictionary::extract_subject(unsigned long id) const {
  return subjects_extra[id];
}

std::string
NaiveDynamicStringDictionary::extract_predicate(unsigned long id) const {
  return predicates_extra[id];
}

std::string
NaiveDynamicStringDictionary::extract_object(unsigned long id) const {
  return objects_extra[id];
}
