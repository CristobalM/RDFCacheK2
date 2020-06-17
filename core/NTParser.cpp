#include <utility>

//
// Created by Cristobal Miranda, 2020
//

#include "NTParser.hpp"
#include <raptor2.h>

#include <fstream>
#include <utility>

#include "EntitiesMapping.hpp"

NTParser::NTParser(std::string input_path,
                   std::shared_ptr<EntitiesMapping> previous_mapping)
    : input_path(std::move(input_path)),
      previous_mapping(std::move(previous_mapping)) {}

NTParser::NTParser(std::string input_path)
    : input_path(input_path),
      previous_mapping(std::make_shared<EntitiesMapping>()) {}

std::vector<std::pair<ulong, ulong>> &NTParser::get_pairs() {
  return points_inserted_debug;
}

Entity::EntityType get_term_type(raptor_term *term) {
  switch (term->type) {
  case RAPTOR_TERM_TYPE_UNKNOWN:
    return Entity::UNKNOWN_ENTITY;
  case RAPTOR_TERM_TYPE_LITERAL:
    return Entity::LITERAL_ENTITY;
  case RAPTOR_TERM_TYPE_URI:
    return Entity::URI_ENTITY;
  case RAPTOR_TERM_TYPE_BLANK:
    return Entity::BLANK_ENTITY;
  default:
    throw std::runtime_error("Invalid entity in get_term_type: " +
                             std::to_string(term->type));
  }
}

std::string get_term_value(raptor_term *term) {
  char *value;
  size_t value_sz;
  value =
      reinterpret_cast<char *>(raptor_term_to_counted_string(term, &value_sz));
  auto result = std::string(value, value_sz);
  free(value);

  return result;
}

struct PairNTParserNResult {
  NTParser *nt_parser;
  NTParsedResult *nt_parsed_result;
};

int pred_cont = 0;

void nt_parser_statement_handler(void *_ntpair,
                                 const raptor_statement *statement) {
  auto *nt_pair = static_cast<PairNTParserNResult *>(_ntpair);
  raptor_term *subject = statement->subject;
  raptor_term *predicate = statement->predicate;
  raptor_term *object = statement->object;

  auto predicate_value = get_term_value(predicate);
  if (!nt_pair->nt_parser->should_add_predicate(predicate_value)) {
    return;
  }

  auto subject_value = get_term_value(subject);
  auto object_value = get_term_value(object);

  if (pred_cont % 10000 == 0) {
    std::cout << "on predicate: " << predicate_value
              << ", with S:" << subject_value << ", O:" << object_value << ", #"
              << pred_cont << std::endl;
  }

  pred_cont++;

  auto subject_id = nt_pair->nt_parsed_result->entities_mapping->add_subject(
      subject_value, get_term_type(subject));
  auto predicate_id =
      nt_pair->nt_parsed_result->entities_mapping->add_predicate(
          predicate_value, get_term_type(predicate));
  auto object_id = nt_pair->nt_parsed_result->entities_mapping->add_object(
      object_value, get_term_type(object));

  auto &cache = *nt_pair->nt_parsed_result->predicates_index_cache;
  if (!cache.has_predicate(predicate_id)) {
    cache.add_predicate(predicate_id);
  }

  auto &k2tree = cache.get_k2tree(predicate_id);
  k2tree.insert(subject_id, object_id);
}

std::unique_ptr<NTParsedResult> NTParser::parse() {
  raptor_world *world = raptor_new_world();
  raptor_parser *parser = raptor_new_parser(world, "ntriples");
  std::unique_ptr<NTParsedResult> result;

  if (previous_mapping == nullptr) {
    result = std::make_unique<NTParsedResult>();
  } else {
    result = std::make_unique<NTParsedResult>(previous_mapping);
  }

  PairNTParserNResult pair_to_pass{};
  pair_to_pass.nt_parsed_result = result.get();
  pair_to_pass.nt_parser = this;

  raptor_parser_set_statement_handler(
      parser, (void *)&pair_to_pass,
      (raptor_statement_handler)nt_parser_statement_handler);

  // FILE *stream = fopen(input_path.c_str(), "rb");

  raptor_parser_parse_start(parser, nullptr);

  std::ifstream ifstream(input_path, std::ifstream::binary);
  std::vector<char> buffer(4096, 0);
  while (ifstream.read(buffer.data(), buffer.size())) {
    raptor_parser_parse_chunk(parser,
                              reinterpret_cast<unsigned char *>(buffer.data()),
                              (size_t)ifstream.gcount(), 0);
  }

  // char uri_str[] = "http://www.wikidata.org/entity/";
  // raptor_uri *uri = raptor_new_uri(world, reinterpret_cast<unsigned
  // char*>(uri_str));

  // raptor_parser_parse_file_stream(parser, stream, nullptr, uri);

  // fclose(stream);
  raptor_free_parser(parser);
  raptor_free_world(world);

  return result;
}
void NTParser::set_predicates(std::vector<std::string> &predicates_vec) {
  for (auto &predicate : predicates_vec) {
    this->predicates[predicate] = true;
  }
  has_predicates = true;
}

bool NTParser::should_add_predicate(const std::string &predicate) {
  return !has_predicates || predicates.find(predicate) != predicates.end();
}

void NTParser::insert_debug(ulong col, ulong row) {
  points_inserted_debug.emplace_back(col, row);
}

NTParsedResult::NTParsedResult(std::shared_ptr<EntitiesMapping> mapping)
    : entities_mapping(std::move(mapping)),
      predicates_index_cache(std::make_unique<PredicatesIndexCache>()) {}
NTParsedResult::NTParsedResult() {}
