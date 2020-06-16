//
// Created by Cristobal Miranda, 2020
//

#include <fstream>
#include <getopt.h>
#include <iostream>
#include <string>

#include <SDEntitiesMapping.hpp>

#include <RadixIteratorDictString.hpp>
#include <RadixTree.hpp>
#include <StringDictionaryPFC.h>

struct parsed_options {
  std::string subjects_radix_set_file_input;
  std::string predicates_radix_set_file_input;
  std::string objects_radix_set_file_input;

  std::string subjects_sd_file_output;
  std::string predicates_sd_file_output;
  std::string objects_sd_file_output;
};

parsed_options parse_cmline(int argc, char **argv);

void print_help();

void check_opt(bool value, const std::string &opt_name);

int main(int argc, char **argv) {
  parsed_options parsed = parse_cmline(argc, argv);

  const int bsize = 100;

  {
    RadixTree<> subjects_radix_tree;
    std::ifstream ifs(parsed.subjects_radix_set_file_input, std::ios::binary);
    subjects_radix_tree.deserialize(ifs);
    // RadixIteratorDictString<> it_sub(subjects_radix_tree);
    auto *it = dynamic_cast<IteratorDictString *>(
        new RadixIteratorDictString<>(subjects_radix_tree));
    StringDictionaryPFC sd_sub(it, 100);
    std::ofstream ofs(parsed.subjects_sd_file_output);
    sd_sub.save(ofs);
  }

  {
    RadixTree<> predicates_radix_tree;
    std::ifstream ifs(parsed.predicates_radix_set_file_input, std::ios::binary);
    predicates_radix_tree.deserialize(ifs);
    auto *it = dynamic_cast<IteratorDictString *>(
        new RadixIteratorDictString<>(predicates_radix_tree));
    StringDictionaryPFC sd_preds(dynamic_cast<IteratorDictString *>(it), 100);
    std::ofstream ofs(parsed.predicates_sd_file_output);
    sd_preds.save(ofs);
  }

  {
    RadixTree<> objects_radix_tree;
    std::ifstream ifs(parsed.objects_radix_set_file_input, std::ios::binary);
    objects_radix_tree.deserialize(ifs);
    auto *it = dynamic_cast<IteratorDictString *>(
        new RadixIteratorDictString<>(objects_radix_tree));
    StringDictionaryPFC sd_objs(dynamic_cast<IteratorDictString *>(it), 100);
    std::ofstream ofs(parsed.objects_sd_file_output);
    sd_objs.save(ofs);
  }
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "s:p:o:S:P:O";
  struct option long_options[] = {
      {"subjects-radix-set-file-input", required_argument, nullptr, 's'},
      {"predicates-radix-set-file-input", required_argument, nullptr, 'p'},
      {"objects-radix-set-file-input", required_argument, nullptr, 'o'},

      {"subjects-sd-file-output", required_argument, nullptr, 'S'},
      {"predicates-sd-file-output", required_argument, nullptr, 'P'},
      {"objects-sd-file-output", required_argument, nullptr, 'O'}};

  int opt, opt_index;

  bool opts_bools[] = {false, false, false, false, false, false};

  parsed_options out{};
  while ((
      opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
    if (opt == -1) {
      break;
    }

    switch (opt) {
    case 's':
      out.subjects_radix_set_file_input = optarg;
      opts_bools[0] = true;
      break;
    case 'p':
      out.predicates_radix_set_file_input = optarg;
      opts_bools[1] = true;
      break;
    case 'o':
      out.objects_radix_set_file_input = optarg;
      opts_bools[2] = true;
      break;

    case 'S':
      out.subjects_sd_file_output = optarg;
      opts_bools[3] = true;
      break;
    case 'P':
      out.predicates_sd_file_output = optarg;
      opts_bools[4] = true;
      break;
    case 'O':
      out.objects_sd_file_output = optarg;
      opts_bools[5] = true;
      break;

    case 'h': // to implement
    case '?':
    default:
      print_help();
      break;
    }
  }

  for (int i = 0; i < 6; i++) {
    check_opt(opts_bools[i], long_options[i].name);
  }

  return out;
}

void check_opt(bool value, const std::string &opt_name) {
  if (!value) {
    std::cerr << "Missing option --" << opt_name << "\n" << std::endl;
    print_help();
    exit(1);
  }
}

void print_help() {
  std::cout << "--subjects-radix-set-file-input\t(-s)\t\t(string-required)\n"
               "--predicates-radix-set-file-input\t(-p)\t\t(string-required)\n"
               "--objects-radix-set-file-input\t(-o)\t\t(string-required)\n"

               "--subjects-sd-file-output\t(-S)\t\t(string-required)\n"
               "--predicates-sd-file-output\t(-P)\t\t(string-required)\n"
               "--objects-sd-file-output\t(-O)\t\t(string-required)\n"
            << std::endl;
}