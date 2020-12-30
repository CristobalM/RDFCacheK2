
#include <string>
#include <vector>
#include <algorithm>

#include "SDBuilder.hpp"

#include <StringDictionaryHASHRPDAC.h>
#include <StringDictionaryHASHRPDACBlocks.h>
#include <StringDictionaryHTFC.h>
#include <StringDictionaryPFC.h>
#include <StringDictionaryRPDAC.h>

#include <iterators/IteratorDictString.h>
#include <iterators/IteratorDictStringPlain.h>

#include <base64.h>

struct BufferedData {
  unsigned char *data;
  size_t size;
};

BufferedData put_in_buffer(std::vector<std::string> &&input_vec,
                           const size_t total_size);

SDBuilder::SDBuilder(SDType sd_type, bool base_64_data, SDInput extra_input)
    : sd_type(sd_type), base_64_data(base_64_data), extra_input(extra_input) {}

std::unique_ptr<StringDictionary> SDBuilder::build(std::istream &input_stream) {
  std::string line;
  std::vector<std::string> data_holder;
  size_t bytes_used = 0;

  while (std::getline(input_stream, line)) {
    std::string decoded;
    if (base_64_data)
      decoded = base64_decode(line, true);
    else
      decoded = line;
    bytes_used += decoded.size() + 1;
    data_holder.push_back(std::move(decoded));
  }

  std::sort(data_holder.begin(), data_holder.end());

  auto buffered_data = put_in_buffer(std::move(data_holder), bytes_used);


  std::unique_ptr<StringDictionary> string_dictionary;

  // this is deleted internally by string dictionaries at some point, don't delete here!
  // that is a bad practice that must be changed in the string dictionaries lib
  auto *it = dynamic_cast<IteratorDictString *>(
      new IteratorDictStringPlain(buffered_data.data, buffered_data.size));

  switch (sd_type) {
  case SDType::PFC:
    string_dictionary =
        std::make_unique<StringDictionaryPFC>(it, extra_input.bucket_size);
    break;
  case SDType::HTFC:
    string_dictionary =
        std::make_unique<StringDictionaryHTFC>(it, extra_input.bucket_size);
    break;
  case SDType::HRPDAC:
    string_dictionary =
        std::make_unique<StringDictionaryHASHRPDAC>(it, buffered_data.size, 25);
    break;
  case SDType::HRPDACBlocks:
    string_dictionary = std::make_unique<StringDictionaryHASHRPDACBlocks>(
        dynamic_cast<IteratorDictStringPlain *>(it), buffered_data.size, 25,
        extra_input.cut_size, extra_input.thread_count);
    break;
  case SDType::RPDAC:
    string_dictionary = std::make_unique<StringDictionaryRPDAC>(it);
    break;

  default:
    throw std::runtime_error("SDBuilder::build : Invalid SD Type " +
                             std::to_string(sd_type));
  }

  return string_dictionary;
}

BufferedData put_in_buffer(std::vector<std::string> &&input_vec,
                           const size_t total_size) {
  auto *data = new unsigned char[total_size]();

  
  unsigned long right_pos_exclusive = total_size - 1;
  const long last_i = input_vec.size() - 1;
  for (long i = last_i; i >= 0; i--) {
    auto &current_str = input_vec[i];

    unsigned long left_pos_inclusive = right_pos_exclusive - current_str.size();

    memcpy(data + left_pos_inclusive, current_str.data(), current_str.size());

    right_pos_exclusive -= current_str.size()+1;

    input_vec.pop_back();
  }
  BufferedData out{};
  out.data = data;
  out.size = total_size;

  return out;
}
