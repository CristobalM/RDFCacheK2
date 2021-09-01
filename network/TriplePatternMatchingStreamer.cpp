//
// Created by cristobal on 9/1/21.
//

#include "TriplePatternMatchingStreamer.hpp"


TriplePatternMatchingStreamer::TriplePatternMatchingStreamer(
    int channel_id, int pattern_channel_id,
    const proto_msg::TripleNode triple_pattern_node, Cache *cache, TimeControl *time_control,
    unsigned long threshold_part_size)
    : channel_id(channel_id), pattern_channel_id(pattern_channel_id),
    triple_pattern_node(std::move(triple_pattern_node)), cache(cache),
     time_control(time_control),
    threshold_part_size(threshold_part_size), first(true), finished(false) {
  initialize_scanner();
}

proto_msg::CacheResponse TriplePatternMatchingStreamer::get_next_response() {
  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(proto_msg::MessageType::STREAM_OF_TRIPLES_MATCHING_PATTERN_RESPONSE);
  auto *stream_response =
      cache_response.mutable_stream_of_triples_matching_pattern_response();
  unsigned long acc_size = 0;
  if (first) {
    if (subject_variable) {
      auto subject_var_name = triple_pattern_node.subject().term_value();
      acc_size += subject_var_name.size() * sizeof(char);
      stream_response->mutable_variables()->Add(std::move(subject_var_name));
    }
    if (object_variable) {
      auto object_var_name = triple_pattern_node.object().term_value();
      acc_size += object_var_name.size() * sizeof(char);
      stream_response->mutable_variables()->Add(std::move(object_var_name));
    }
    first = false;
  }

  stream_response->set_channel_id(channel_id);
  stream_response->set_pattern_channel_id(pattern_channel_id);


  while (k2tree_scanner->has_next()) {
    if(!time_control->tick()){
      return timeout_proto_response();
    }
    auto matching_pair_so = k2tree_scanner->next();
    auto *matching_values = stream_response->mutable_matching_values()->Add();
    if (subject_variable) {
      auto subject_resource = cache->extract_resource(matching_pair_so.first);
      acc_size += subject_resource.value.size() * sizeof(char);
      acc_size += sizeof(subject_resource.resource_type);
      matching_values->mutable_single_match()->Add(
          resource_to_term(std::move(subject_resource)));
    }
    if (object_variable) {
      auto object_resource = cache->extract_resource(matching_pair_so.second);
      acc_size += object_resource.value.size() * sizeof(char);
      acc_size += sizeof(object_resource.resource_type);
      matching_values->mutable_single_match()->Add(
          resource_to_term(std::move(object_resource)));
    }

    if(acc_size > threshold_part_size){
      break;
    }
  }

  if(!k2tree_scanner->has_next()){
    stream_response->set_last_result(true);
    set_finished();
  }
  else{
    stream_response->set_last_result(false);
  }

  return cache_response;
}
int TriplePatternMatchingStreamer::get_pattern_channel_id() { return pattern_channel_id; }
int TriplePatternMatchingStreamer::get_channel_id() { return channel_id; }
bool TriplePatternMatchingStreamer::all_sent() { return finished; }

void TriplePatternMatchingStreamer::initialize_scanner() {
  subject_variable = triple_pattern_node.subject().term_type() ==
                     proto_msg::TermType::VARIABLE;
  object_variable =
      triple_pattern_node.object().term_type() == proto_msg::TermType::VARIABLE;

  auto predicate_id = cache->get_pcm().get_resource_index(
      RDFResource(triple_pattern_node.predicate()));

  auto fetch_result =
      cache->get_pcm().get_predicates_index_cache().fetch_k2tree(predicate_id);
  if (!fetch_result.exists()) {
    k2tree_scanner = cache->get_pcm().create_null_k2tree_scanner();
    return;
  }

  auto &k2tree = fetch_result.get_mutable();

  if (subject_variable && object_variable) {
    k2tree_scanner = k2tree.create_full_scanner();
  } else if (subject_variable) {
    auto object_id = cache->get_pcm().get_resource_index(
        RDFResource(triple_pattern_node.object()));
    k2tree_scanner = k2tree.create_band_scanner(
        object_id, K2TreeScanner::BandType::ROW_BAND_TYPE);
  } else {
    auto subject_id = cache->get_pcm().get_resource_index(
        RDFResource(triple_pattern_node.subject()));
    k2tree_scanner = k2tree.create_band_scanner(
        subject_id, K2TreeScanner::BandType::COLUMN_BAND_TYPE);
  }
}
proto_msg::RDFTerm
TriplePatternMatchingStreamer::resource_to_term(RDFResource &&resource) {
  proto_msg::RDFTerm rdf_term;
  rdf_term.set_term_type(resource.get_proto_type());
  rdf_term.set_term_value(std::move(resource.value));
  return rdf_term;
}
void TriplePatternMatchingStreamer::set_finished() {
  finished = true;
}
proto_msg::CacheResponse
TriplePatternMatchingStreamer::timeout_proto_response() {
  set_finished();
  proto_msg::CacheResponse result;
  result.set_response_type(proto_msg::TIMED_OUT_RESPONSE);
  result.mutable_error_response();
  return result;
}
