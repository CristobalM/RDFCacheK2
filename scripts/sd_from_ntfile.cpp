//
// Created by Cristobal Miranda, 2020
//

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <getopt.h>

#include <QueueIteratorDictString.hpp>
#include <SDFeed.hpp>

#include <raptor2.h>
#include <raptor_util.hpp>

#include <StringDictionaryHTFC.h>
#include <StringDictionaryPFC.h>

struct parsed_options {
  std::string input_file;
  std::string output_file;
};

parsed_options parse_cmline(int argc, char **argv);
void print_help();
void process_nt_file(const std::string &input_file_path,
                     const std::string &output_file_path);
void statement_handler(void *feeds_holder_ptr,
                       const raptor_statement *statement);
void print_stats();

unsigned long bytes_processed = 0;
unsigned long strings_processed = 0;

unsigned long bytes_stored = 0;

unsigned long strings_processed_reset_th = 0;

const unsigned long RESET_TH = 1000000;

struct FeedsHolder {
  SDFeed &subjects_feed;
  SDFeed &predicates_feed;
  SDFeed &objects_feed;

  FeedsHolder(SDFeed &subjects_feed, SDFeed &predicates_feed,
              SDFeed &objects_feed)
      : subjects_feed(subjects_feed), predicates_feed(predicates_feed),
        objects_feed(objects_feed) {}
};

int main(int argc, char **argv) {
  parsed_options p_options = parse_cmline(argc, argv);

  process_nt_file(p_options.input_file, p_options.output_file);

  return 0;
}

class ISDBuildJob {
public:
  virtual void wait_until_done() = 0;
  virtual ~ISDBuildJob() = default;
};

template <class SD> class SDBuildJob : public ISDBuildJob {
  std::unique_ptr<std::thread> thread;
  SDFeed &sd_feed;
  std::string output_filename;

public:
  SDBuildJob(SDFeed &sd_feed, std::string output_filename)
      : sd_feed(sd_feed), output_filename(std::move(output_filename)) {
    sd_feed.set_finished(false);
  }

  virtual ~SDBuildJob() {}

  void start() {
    auto *it = dynamic_cast<IteratorDictString *>(
        new QueueIteratorDictString(&sd_feed));
    thread = std::make_unique<std::thread>(&SDBuildJob<SD>::job, this, it);
  }

  void job(IteratorDictString *it) {
    auto sd = create_sd(it);
    std::ofstream ofs(output_filename, std::ofstream::binary);
    sd->save(ofs);
  }

  std::unique_ptr<StringDictionary> create_sd(IteratorDictString *it);

  void wait_until_done() override { thread->join(); };
};

constexpr size_t bucket_sz = 4096;

template <>
std::unique_ptr<StringDictionary>
SDBuildJob<StringDictionaryHTFC>::create_sd(IteratorDictString *it) {
  return std::unique_ptr<StringDictionary>(
      new StringDictionaryHTFC(it, bucket_sz));
}

template <>
std::unique_ptr<StringDictionary>
SDBuildJob<StringDictionaryPFC>::create_sd(IteratorDictString *it) {
  return std::unique_ptr<StringDictionary>(
      new StringDictionaryPFC(it, bucket_sz));
}

template <class SD>
std::unique_ptr<ISDBuildJob>
start_building_sd(SDFeed &sd_feed, const std::string &output_fname) {
  auto job = std::make_unique<SDBuildJob<SD>>(sd_feed, output_fname);
  job->start();
  return job;
}

void process_nt_file(const std::string &input_file_path,
                     const std::string &output_file_path) {
  SDFeed subjects_feed;
  SDFeed predicates_feed;
  SDFeed objects_feed;

  FeedsHolder feeds_holder(subjects_feed, predicates_feed, objects_feed);
  raptor_world *world = raptor_new_world();
  raptor_parser *parser = raptor_new_parser(world, "ntriples");

  raptor_parser_set_statement_handler(
      parser, (void *)&feeds_holder,
      (raptor_statement_handler)statement_handler);

  raptor_parser_parse_start(parser, nullptr);

  std::ifstream ifstream(input_file_path, std::ifstream::binary);

  if (ifstream.fail()) {
    std::cerr << "Error opening NT file " << input_file_path << std::endl;
    exit(1);
  }

  auto subjects_job = start_building_sd<StringDictionaryPFC>(
      subjects_feed, output_file_path + ".subjects.sd");
  auto predicates_job = start_building_sd<StringDictionaryPFC>(
      predicates_feed, output_file_path + ".predicates.sd");
  auto objects_job = start_building_sd<StringDictionaryHTFC>(
      objects_feed, output_file_path + ".objects.sd");

  std::vector<char> buffer(4096, 0);
  while (ifstream.read(buffer.data(), buffer.size())) {
    raptor_parser_parse_chunk(parser,
                              reinterpret_cast<unsigned char *>(buffer.data()),
                              (size_t)ifstream.gcount(), 0);
  }

  subjects_feed.set_finished(true);
  predicates_feed.set_finished(true);
  objects_feed.set_finished(true);

  std::cout << "Done. \t";
  print_stats();

  raptor_free_parser(parser);
  raptor_free_world(world);

  subjects_job->wait_until_done();
  predicates_job->wait_until_done();
  objects_job->wait_until_done();
}

void statement_handler(void *feeds_holder_ptr,
                       const raptor_statement *statement) {
  auto &feeds_holder = *reinterpret_cast<FeedsHolder *>(feeds_holder_ptr);

  auto &subjects_feed = feeds_holder.subjects_feed;
  auto &predicates_feed = feeds_holder.predicates_feed;
  auto &objects_feed = feeds_holder.objects_feed;

  raptor_term *subject = statement->subject;
  raptor_term *predicate = statement->predicate;
  raptor_term *object = statement->object;

  auto subject_value = get_term_value(subject);
  auto predicate_value = get_term_value(predicate);
  auto object_value = get_term_value(object);

  subjects_feed.push(subject_value);
  predicates_feed.push(predicate_value);
  objects_feed.push(object_value);

  bytes_stored +=
      subject_value.size() + predicate_value.size() + subject_value.size();

  strings_processed += 3;
  strings_processed_reset_th += 3;
  bytes_processed +=
      predicate_value.size() + subject_value.size() + object_value.size();

  if (strings_processed_reset_th >= RESET_TH) {
    strings_processed_reset_th %= RESET_TH;
    print_stats();
  }
}

void print_stats() {
  std::cout << "Strings processed: " << strings_processed << "\t"
            << "MBytes processed: " << bytes_processed / 1'000'000 << "\t"
            << "MBytes stored (virtually): " << bytes_stored / 1'000'000
            << std::endl;
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "f:o:";
  struct option long_options[] = {
      {"input-file", required_argument, nullptr, 'f'},
      {"output-file", required_argument, nullptr, 'o'}};

  int opt, opt_index;

  bool has_input = false;
  bool has_output = false;

  parsed_options out{};
  while ((
      opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
    if (opt == -1) {
      break;
    }

    switch (opt) {
    case 'f':
      out.input_file = optarg;
      has_input = true;
      break;
    case 'o':
      out.output_file = optarg;
      has_output = true;
      break;
    case 'h': // to implement
    case '?':
    default:
      print_help();
      break;
    }
  }

  if (!has_input) {
    std::cerr << "Missing option --input-data\n" << std::endl;
    print_help();
    exit(1);
  }

  if (!has_output) {
    std::cerr << "Missing option --output-data\n" << std::endl;
    print_help();
    exit(1);
  }

  return out;
}

void print_help() {
  std::cout << "--input-file\t(-f)\t\t(string-required)\n"
            << "--output-file\t(-o)\t\t(string-required)\n"
            << std::endl;
}
