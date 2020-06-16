//
// Created by Cristobal Miranda, 2020
//

#include <fstream>
#include <getopt.h>
#include <iostream>
#include <string>

#include <SDEntitiesMapping.hpp>

#include <StringDictionaryHASHRPDAC.h>
#include <RadixTree.hpp>
#include <RadixIteratorDictString.hpp>

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
  std::cout << "hola" << std::endl;
  parsed_options parsed = parse_cmline(argc, argv);
  std::cout << "subjects_radix_set_file_input: " << parsed.subjects_radix_set_file_input << std::endl;
  std::cout << "predicates_radix_set_file_input: " << parsed.predicates_radix_set_file_input << std::endl;
  std::cout << "objects_radix_set_file_input: " << parsed.objects_radix_set_file_input << std::endl;
  RadixTree<> subjects_radix_tree, predicates_radix_tree, objects_radix_tree;

  {
    std::ifstream ifs(parsed.subjects_radix_set_file_input, std::ios::binary);
    subjects_radix_tree.deserialize(ifs);
  }

  {
    std::ifstream ifs(parsed.predicates_radix_set_file_input, std::ios::binary);
    predicates_radix_tree.deserialize(ifs);
  }

  {
    std::ifstream ifs(parsed.objects_radix_set_file_input, std::ios::binary);
    objects_radix_tree.deserialize(ifs);
  }


  RadixIteratorDictString<> it_sub(subjects_radix_tree);
  RadixIteratorDictString<> it_preds(predicates_radix_tree);
  RadixIteratorDictString<> it_objs(objects_radix_tree);

  StringDictionaryHASHRPDAC sd_sub(&it_sub, 0, 1000);
  StringDictionaryHASHRPDAC sd_preds(&it_preds, 0, 1000);
  StringDictionaryHASHRPDAC sd_objs(&it_objs, 0, 1000);


  {
    std::ofstream ofs(parsed.subjects_sd_file_output);
    sd_sub.save(ofs);
  }

  {
    std::ofstream ofs(parsed.predicates_sd_file_output);
    sd_preds.save(ofs);
  }

  {
    std::ofstream ofs(parsed.objects_sd_file_output);
    sd_objs.save(ofs);
  }



  /*
  SDEntitiesMapping<StringDictionaryHASHRPDAC> sd_em(sub_ifs, pred_ifs, obj_ifs);
  sd_em.save(parsed.subjects_sd_file_output, parsed.predicates_sd_file_output, parsed.objects_sd_file_output);
   */
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "s:p:o:S:P:O";
  struct option long_options[] = {
          {"subjects-radix-set-file-input",   required_argument, nullptr, 's'},
          {"predicates-radix-set-file-input", required_argument, nullptr, 'p'},
          {"objects-radix-set-file-input",    required_argument, nullptr, 'o'},

          {"subjects-sd-file-output",         required_argument, nullptr, 'S'},
          {"predicates-sd-file-output",       required_argument, nullptr, 'P'},
          {"objects-sd-file-output",          required_argument, nullptr, 'O'}
  };

  int opt, opt_index;

  bool opts_bools[] = {false, false, false, false, false, false};

  std::cout << "going to parse" << std::endl;
  parsed_options out{};
  while ((
          opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
    if (opt == -1) {
      break;
    }


    switch (opt) {
      case 's':
        std::cout << "on s" << std::endl;
        out.subjects_radix_set_file_input = optarg;
        opts_bools[0] = true;
        break;
      case 'p':
        std::cout << "on p" << std::endl;
        out.predicates_radix_set_file_input = optarg;
        opts_bools[1] = true;
        break;
      case 'o':
        std::cout << "on o" << std::endl;
        out.objects_radix_set_file_input = optarg;
        opts_bools[2] = true;
        break;

      case 'S':
        std::cout << "on S" << std::endl;
        out.subjects_sd_file_output = optarg;
        opts_bools[3] = true;
        break;
      case 'P':
        std::cout << "on P" << std::endl;
        out.predicates_sd_file_output = optarg;
        opts_bools[4] = true;
        break;
      case 'O':
        std::cout << "on O" << std::endl;
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

  std::cout << "going to check" << std::endl;

  for (int i = 0; i < 6; i++) {
    check_opt(opts_bools[i], long_options[i].name);
    std::cout << "Checking: " << long_options[i].name << std::endl;
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
  std::cout
          << "--subjects-radix-set-file-input\t(-s)\t\t(string-required)\n"
             "--predicates-radix-set-file-input\t(-p)\t\t(string-required)\n"
             "--objects-radix-set-file-input\t(-o)\t\t(string-required)\n"

             "--subjects-sd-file-output\t(-S)\t\t(string-required)\n"
             "--predicates-sd-file-output\t(-P)\t\t(string-required)\n"
             "--objects-sd-file-output\t(-O)\t\t(string-required)\n"
          << std::endl;
}