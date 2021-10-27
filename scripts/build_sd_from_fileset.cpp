//
// Created by cristobal on 24-10-21.
//

#include <stdexcept>
#include <string>

#include <getopt.h>
#include <serialization_util.hpp>
#include <triple_external_sort.hpp>
#include <BinaryTriplesToSingleStringFile.hpp>
#include <FileIStream.hpp>
#include <FileOStream.hpp>
#include <external_sort.hpp>
#include <LightStringSortConnector.hpp>
#include <system_info.hpp>
#include <StringDictionary.h>
#include <StringDictionaryHASHRPDACBlocks.h>
#include "SDEnum.hpp"


struct parsed_options {
    std::string input_file;
    std::string output_file;
    STRING_DICTIONARY string_dictionary;
    int thread_count;
    unsigned long cut_size;
    int bucket_size;
};

parsed_options parse_cmline(int argc, char **argv);

struct BufferedData {
    unsigned char *data;
    size_t size;
};


BufferedData put_in_buffer(std::vector<std::string> &input_vec,
                           const size_t total_size);

std::unique_ptr<StringDictionary> string_dict_from_parsed(parsed_options &parsed_options);

int main(int argc, char **argv) {
    auto parsed = parse_cmline(argc, argv);
    auto string_dictionary = string_dict_from_parsed(parsed);
    FileOStream file_ostream(parsed.output_file, std::ios::out);
    string_dictionary->save(file_ostream.get_stream());
    return 0;
}

std::unique_ptr<StringDictionary> string_dict_from_parsed(parsed_options &parsed_options) {

    std::string line;
    std::vector<std::string> data_holder;
    size_t bytes_used = 0;

    FileIStream file_istream(parsed_options.input_file, std::ios::in);

    while (std::getline(file_istream.get_stream(), line)) {
        bytes_used += line.size() + 1;
        data_holder.push_back(line);
    }
    auto buffered_data = put_in_buffer(data_holder, bytes_used);

    std::unique_ptr<StringDictionary> string_dictionary;

    // this is deleted internally by string dictionaries at some point, don't
    // delete here! that is a bad practice that must be changed in the string
    // dictionaries lib
    auto *it = dynamic_cast<IteratorDictString *>(
            new IteratorDictStringPlain(buffered_data.data, buffered_data.size));

    switch (parsed_options.string_dictionary) {
        case SD_HRPDACBLOCKS: {
            string_dictionary = std::make_unique<StringDictionaryHASHRPDACBlocks>(
                    dynamic_cast<IteratorDictStringPlain *>(it), buffered_data.size, 25,
                    parsed_options.cut_size, parsed_options.thread_count);
            break;
        }
        case SD_PFC: {
            string_dictionary =
                    std::make_unique<StringDictionaryPFC>(it, parsed_options.bucket_size);
            break;
        }
    }

    return string_dictionary;
}

parsed_options parse_cmline(int argc, char **argv) {
    const char short_options[] = "i:o:s:t::c::b::";
    struct option long_options[] = {
            {"input-file",        required_argument, nullptr, 'i'},
            {"output-file",       required_argument, nullptr, 'o'},
            {"string-dictionary", required_argument, nullptr, 's'},

            {"thread-count",      optional_argument, nullptr, 't'},
            {"cut-size",          optional_argument, nullptr, 'c'},
            {"bucket-size",       optional_argument, nullptr, 'b'}
    };

    int opt, opt_index;

    bool has_input = false;
    bool has_output = false;
    bool has_sd = false;

    bool has_thread_count = false;
    bool has_cut_size = false;
    bool has_bucket_size = false;

    parsed_options out{};

    while ((
            opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
        if (opt == -1) {
            break;
        }
        switch (opt) {
            case 'i':
                out.input_file = optarg;
                has_input = true;
                break;
            case 'o':
                out.output_file = optarg;
                has_output = optarg;
                break;
            case 's': {
                std::string input_sd = optarg;
                std::for_each(input_sd.begin(), input_sd.end(), [](char &c) {
                    c = (char) std::tolower(c);
                });
                STRING_DICTIONARY string_dictionary;

                if (input_sd == "pfc") {
                    string_dictionary = STRING_DICTIONARY::SD_PFC;
                } else if (input_sd == "hrpdacblocks") {
                    string_dictionary = STRING_DICTIONARY::SD_HRPDACBLOCKS;
                } else {
                    throw std::runtime_error("invalid string dictionary " + input_sd);
                }
                out.string_dictionary = string_dictionary;
                has_sd = true;
                break;
            }
            case 't': {
                if (optarg) {
                    out.thread_count = std::stoi(optarg);
                    has_thread_count = true;
                }
                break;
            }
            case 'c': {
                if (optarg) {
                    out.cut_size = std::stoul(optarg);
                    has_cut_size = true;
                }
                break;
            }
            case 'b': {
                if (optarg) {
                    out.bucket_size = std::stoi(optarg);
                    has_bucket_size = true;
                }
                break;
            }
            default:
                break;
        }
    }

    if (!has_input)
        throw std::runtime_error("input-file (i) argument is required");
    if (!has_output)
        throw std::runtime_error("output-file (o) argument is required");
    if (!has_sd)
        throw std::runtime_error("string-dictionary (s) argument is required");


    if (!has_thread_count) {
        out.thread_count = 1;
    }
    if (!has_bucket_size) {
        out.bucket_size = 128;
    }
    if (!has_cut_size) {
        out.cut_size = 1'000'000; // 1 MB
    }

    return out;
}


BufferedData put_in_buffer(std::vector<std::string> &input_vec,
                           const size_t total_size) {
    auto *data = new unsigned char[total_size]();

    unsigned long right_pos_exclusive = total_size - 1;
    const long last_i = input_vec.size() - 1;
    for (long i = last_i; i >= 0; i--) {
        auto &current_str = input_vec[i];

        unsigned long left_pos_inclusive = right_pos_exclusive - current_str.size();

        memcpy(data + left_pos_inclusive, current_str.data(), current_str.size());

        right_pos_exclusive -= current_str.size() + 1;

        input_vec.pop_back();
    }
    BufferedData out{};
    out.data = data;
    out.size = total_size;

    return out;
}
