#include "PredicatesIndexFileBuilder.hpp"
#include "K2TreeMixed.hpp"
#include "external_sort.hpp"
#include "serialization_util.hpp"
#include <unordered_map>


static void write_ktree_with_size(std::iostream &ios, K2TreeMixed &k2tree) {
  uint64_t sz = 0;
  auto bef = ios.tellp();
  write_u64(ios, sz);
  auto start = ios.tellp();
  k2tree.write_to_ostream(ios);
  auto end = ios.tellp();
  sz = end - start;
  ios.seekp(bef);
  write_u64(ios, sz);
  ios.seekp(end);
}

PredicatesCacheMetadata PredicatesIndexFileBuilder::build(std::istream &input_file,
                             std::ostream &output_file, 
                            std::iostream &tmp_stream,
                             uint32_t treedepth,
                             uint32_t max_node_count, uint32_t cut_depth) {
  FileData filedata{};
  filedata.current_triple = 0;
  filedata.size = read_u64(input_file);
  std::unique_ptr<K2TreeMixed> current_k2tree;
  std::vector<uint64_t> predicates_ids;
  uint64_t current_predicate = 0;
  while (!filedata.finished()) {
    auto triple = filedata.read_triple(input_file);
    if (triple.second != current_predicate) {
      if (current_k2tree) {
        write_ktree_with_size(tmp_stream, *current_k2tree);
      }
      current_k2tree =
          std::make_unique<K2TreeMixed>(treedepth, max_node_count, cut_depth);
      current_predicate = triple.second;
      predicates_ids.push_back(current_predicate);
    }
    current_k2tree->insert(triple.first, triple.second);
  }
  if (current_k2tree) {
    write_ktree_with_size(tmp_stream, *current_k2tree);
    current_k2tree = nullptr;
  }

  std::unordered_map<uint64_t, PredicateMetadata> metadata_map;

  uint64_t predicates_count = predicates_ids.size();
  write_u64(output_file, predicates_count);
  auto metadata_start = output_file.tellp();
  for (auto predicate_id : predicates_ids) {
    metadata_map[predicate_id] = PredicateMetadata{};
    metadata_map[predicate_id].write_to_ostream(output_file);
  }

  for (auto predicate_id : predicates_ids) {
    auto k2tree_serialized_size = read_u64(tmp_stream);
    auto offset = output_file.tellp();
    std::vector<char> buf(k2tree_serialized_size);
    tmp_stream.read(buf.data(), k2tree_serialized_size);
    output_file.write(buf.data(), k2tree_serialized_size);
    auto &tree_metadata = metadata_map[predicate_id];
    tree_metadata.predicate_id = predicate_id;
    tree_metadata.tree_offset = offset;
    tree_metadata.tree_size = k2tree_serialized_size;
    tree_metadata.priority = 0;
  }

  auto to_restore = output_file.tellp();
  output_file.seekp(metadata_start);
  for (auto predicate_id : predicates_ids) {
    metadata_map[predicate_id].write_to_ostream(output_file);
  }
  output_file.seekp(to_restore);

  return PredicatesCacheMetadata(std::move(metadata_map),
                                 std::move(predicates_ids));
}

PredicatesCacheMetadata PredicatesIndexFileBuilder::build(std::istream &input_file,
                             std::ostream &output_file, 
                            std::iostream &tmp_stream,
                             K2TreeConfig config){
  return build(input_file, output_file, tmp_stream, config.treedepth, config.max_node_count, config.cut_depth);
}
