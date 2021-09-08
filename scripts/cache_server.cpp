#include <algorithm>
#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>

#include <getopt.h>

#include "CacheArgs.hpp"
#include <Cache.hpp>
#include <CacheServer.hpp>
#include <PredicatesCacheManager.hpp>
#include <SDEntitiesMapping.hpp>
#include <StringDictionaryHASHRPDACBlocks.h>
#include <StringDictionaryPFC.h>

namespace fs = std::filesystem;

struct parsed_options {
  std::string index_file;
  std::string iris_file;
  std::string blanks_file;
  std::string literals_file;
  unsigned long memory_budget_bytes;
  int port;
  int workers_count;

  std::string temp_files_dir;
  unsigned long time_out_ms;

  I_CacheReplacement::REPLACEMENT_STRATEGY replacement_strategy;

  std::string update_log_filename;
};

parsed_options parse_cmline(int argc, char **argv);

void check_exists(const std::string &fname) {
  if (!fs::exists(fname)) {
    throw std::runtime_error("Filename '" + fname + "' does not exist");
  }
}

int main(int argc, char **argv) {

  auto parsed = parse_cmline(argc, argv);

  check_exists(parsed.index_file);
  check_exists(parsed.iris_file);
  check_exists(parsed.blanks_file);
  check_exists(parsed.literals_file);

  std::unique_ptr<ISDManager> sds;
  {
    auto open_mode = std::ios::binary | std::ios::in;

    std::ifstream iris_ifs(parsed.iris_file, open_mode);
    std::ifstream blanks_ifs(parsed.blanks_file, open_mode);
    std::ifstream literals_ifs(parsed.literals_file, open_mode);

    sds = std::make_unique<
        SDEntitiesMapping<StringDictionaryPFC, StringDictionaryPFC,
                          StringDictionaryHASHRPDACBlocks>>(
        iris_ifs, blanks_ifs, literals_ifs);
  }

  auto pcm = std::make_shared<PredicatesCacheManager>(std::move(sds),
                                                      parsed.index_file);

  fs::create_directories(fs::path(parsed.temp_files_dir));

  if (parsed.replacement_strategy ==
      I_CacheReplacement::REPLACEMENT_STRATEGY::NO_CACHING) {
    std::cout << "Loading all predicates..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    pcm->load_all_predicates();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout << "Done loading all predicates, took " << duration.count()
              << " seconds" << std::endl;
  }

  CacheArgs cache_args;
  cache_args.memory_budget_bytes = parsed.memory_budget_bytes;
  cache_args.temp_files_dir = parsed.temp_files_dir;
  cache_args.time_out_ms = parsed.time_out_ms;
  cache_args.replacement_strategy = parsed.replacement_strategy;
  cache_args.update_log_filename = parsed.update_log_filename;

  Cache cache(pcm, cache_args);

  CacheServer server(cache, parsed.port, parsed.workers_count);
  server.start();
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "I:O:i:b:l:m:p:w:t:T:R:";
  struct option long_options[] = {
      {"index-file", required_argument, nullptr, 'I'},
      {"iris-file", required_argument, nullptr, 'i'},
      {"blanks-file", required_argument, nullptr, 'b'},
      {"literals-file", required_argument, nullptr, 'l'},
      {"memory-budget", required_argument, nullptr, 'm'},
      {"port", required_argument, nullptr, 'p'},
      {"workers", required_argument, nullptr, 'w'},
      {"temp-files-dir", required_argument, nullptr, 't'},
      {"timeout-ms", required_argument, nullptr, 'T'},
      {"replacement-strategy", required_argument, nullptr, 'R'},
      {"update-log-filename", required_argument, nullptr, 'U'},
  };

  int opt, opt_index;

  bool has_index = false;
  bool has_iris = false;
  bool has_blanks = false;
  bool has_literals = false;
  bool has_memory_budget = false;
  bool has_port = false;
  bool has_workers = false;
  bool has_temp_files_dir = false;
  bool has_timeout = false;
  bool has_strategy = false;
  bool has_update_log_filename = false;
  parsed_options out{};

  while ((
      opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
    if (opt == -1) {
      break;
    }
    switch (opt) {
    case 'I':
      out.index_file = optarg;
      has_index = true;
      break;
    case 'i':
      out.iris_file = optarg;
      has_iris = true;
      break;
    case 'b':
      out.blanks_file = optarg;
      has_blanks = true;
      break;
    case 'l':
      out.literals_file = optarg;
      has_literals = true;
      break;
    case 'm':
      out.memory_budget_bytes = std::stoul(std::string(optarg));
      has_memory_budget = true;
      break;
    case 'p':
      out.port = std::stoi(std::string(optarg));
      has_port = true;
      break;
    case 'w':
      out.workers_count = std::stoi(std::string(optarg));
      has_workers = true;
      break;
    case 't':
      out.temp_files_dir = optarg;
      has_temp_files_dir = true;
      break;
    case 'T':
      out.time_out_ms = std::stoul(std::string(optarg));
      has_timeout = true;
      break;
    case 'R': {
      std::string replacement_strategy_str(optarg);
      std::for_each(replacement_strategy_str.begin(),
                    replacement_strategy_str.end(),
                    [](char &c) { c = (char)std::tolower(c); });
      if (replacement_strategy_str == "lru") {
        out.replacement_strategy =
            I_CacheReplacement::REPLACEMENT_STRATEGY::LRU;
      } else if (replacement_strategy_str == "frequency") {
        out.replacement_strategy =
            I_CacheReplacement::REPLACEMENT_STRATEGY::FREQUENCY;
      } else {
        out.replacement_strategy =
            I_CacheReplacement::REPLACEMENT_STRATEGY::NO_CACHING;
      }
      has_strategy = true;
    } break;
    case 'U':
      out.update_log_filename = optarg;
      has_update_log_filename = true;
      break;
    default:
      break;
    }
  }

  if (!has_index)
    throw std::runtime_error("index-file (I) argument is required");
  if (!has_iris)
    throw std::runtime_error("iris-file (i) argument is required");
  if (!has_blanks)
    throw std::runtime_error("blanks-file (b) argument is required");
  if (!has_literals)
    throw std::runtime_error("literals-file (l) argument is required");
  if (!has_memory_budget)
    throw std::runtime_error("memory-budget (m) argument is required");
  if (!has_port)
    throw std::runtime_error("port (p) argument is required");
  if (!has_workers)
    throw std::runtime_error("workers (w) argument is required");
  if (!has_temp_files_dir)
    throw std::runtime_error("temp-files-dir (t) argument is required");
  if (!has_timeout)
    throw std::runtime_error("timeout-ms (T) argument is required");
  if (!has_strategy)
    throw std::runtime_error(
        "replacement-strategy (R) (one of: 'LRU','None') argument is required");
  if (!has_update_log_filename)
    throw std::runtime_error("update-log-filename (U) argument is required");

  return out;
}