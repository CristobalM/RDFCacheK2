#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <string>

#include <getopt.h>
#include <stdio.h>

#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

#include "CacheArgs.hpp"
#include "CacheContainerFactory.hpp"
#include "server/CacheServer.hpp"

namespace fs = std::filesystem;
using namespace k2cache;

struct parsed_options {
  std::string index_file;
  std::string node_ids_file;
  std::string mapped_node_ids_filename;

  uint64_t memory_budget_bytes;
  int port;
  int workers_count;
  int timeout_ms;

  I_CacheReplacement::REPLACEMENT_STRATEGY replacement_strategy;

  std::string update_log_filename;
  std::string node_ids_logs_filename;

  bool has_fic;
  bool has_sort_results;

};

parsed_options parse_cmd_line(int argc, char **argv);

void check_exists(const std::string &fname) {
  if (!fs::exists(fname)) {
    throw std::runtime_error("Filename '" + fname + "' does not exist");
  }
}

void signal_handler(int sig){
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

int main(int argc, char **argv) {
  signal(SIGSEGV, signal_handler);   // install our handler
  signal(7, signal_handler);   // install our handler


  auto parsed = parse_cmd_line(argc, argv);

  check_exists(parsed.index_file);
  check_exists(parsed.node_ids_file);

  CacheArgs cache_args;
  cache_args.index_filename = parsed.index_file;
  cache_args.memory_budget_bytes = parsed.memory_budget_bytes;
  cache_args.replacement_strategy = parsed.replacement_strategy;
  cache_args.update_log_filename = parsed.update_log_filename;
  cache_args.node_ids_filename = parsed.node_ids_file;
  cache_args.mapped_node_ids_filename = parsed.mapped_node_ids_filename;
  cache_args.node_ids_logs_filename = parsed.node_ids_logs_filename;
  cache_args.has_fic = parsed.has_fic;
  cache_args.has_sort_results = parsed.has_sort_results;
  cache_args.timeout_ms = parsed.timeout_ms;

  std::cout << "has fic: " << (cache_args.has_fic ? "YES": "NO") << std::endl;

  auto cache = CacheContainerFactory::create(cache_args);

  CacheServer server(*cache, parsed.port, parsed.workers_count);
  server.start();
}

parsed_options parse_cmd_line(int argc, char **argv) {
  const char short_options[] = "I:N:M:L:O:T:m:p:w:R:U:F::S::";
  struct option long_options[] = {
      {"index-file", required_argument, nullptr, 'I'},
      {"node-ids-file", required_argument, nullptr, 'N'},
      {"mapped-node-ids-file", required_argument, nullptr, 'M'},
      {"node-ids-logs-file", required_argument, nullptr, 'L'},
      {"memory-budget", required_argument, nullptr, 'm'},
      {"port", required_argument, nullptr, 'p'},
      {"workers", required_argument, nullptr, 'w'},
      {"replacement-strategy", required_argument, nullptr, 'R'},
      {"update-log-filename", required_argument, nullptr, 'U'},
      {"fic", optional_argument, nullptr, 'F'},
      {"sort-results", optional_argument, nullptr, 'S'},
      {"timeout-ms", optional_argument, nullptr, 'T'},
  };

  int opt, opt_index;

  bool has_index = false;
  bool has_node_ids = false;
  bool has_mapped_node_ids = false;
  bool has_node_ids_logs = false;
  bool has_memory_budget = false;
  bool has_port = false;
  bool has_workers = false;
  bool has_strategy = false;
  bool has_update_log_filename = false;
  bool has_timeout = false;
  parsed_options out{};

  out.has_fic = false;
  out.has_sort_results = false;

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
    case 'N':
      out.node_ids_file = optarg;
      has_node_ids = true;
      break;
    case 'M':
      out.mapped_node_ids_filename = optarg;
      has_mapped_node_ids = true;
      break;
    case 'L':
      out.node_ids_logs_filename = optarg;
      has_node_ids_logs = true;
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
    case 'F':
      out.has_fic = true;
      break;
    case 'S':
      out.has_sort_results = true;
      break;
    case 'T':
      has_timeout = true;
      out.timeout_ms = std::stoi(std::string(optarg));
      break;
    default:
      break;
    }
  }

  if (!has_index)
    throw std::runtime_error("index-file (I) argument is required");
  if (!has_node_ids)
    throw std::runtime_error("node-ids-file (N) argument is required");
  if (!has_mapped_node_ids)
    throw std::runtime_error("mapped-node-ids-file (M) argument is required");
  if (!has_node_ids_logs)
    throw std::runtime_error("node-ids-logs-file (L) argument is required");
  if (!has_memory_budget)
    throw std::runtime_error("memory-budget (m) argument is required");
  if (!has_port)
    throw std::runtime_error("port (p) argument is required");
  if (!has_workers)
    throw std::runtime_error("workers (w) argument is required");
  if (!has_strategy)
    throw std::runtime_error(
        "replacement-strategy (R) (one of: 'LRU','None') argument is required");
  if (!has_update_log_filename)
    throw std::runtime_error("update-log-filename (U) argument is required");
  if (!has_timeout)
    throw std::runtime_error("timeout-ms (T) argument is required");

  return out;
}