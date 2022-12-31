#include "PredicatesIndexFileBuilder.hpp"
#include "hashing.hpp"
#include "k2tree/K2TreeBulkOp.hpp"
#include "k2tree/K2TreeMixed.hpp"
#include "serialization_util.hpp"
#include "triple_external_sort.hpp"
#include <set>
#include <stdexcept>
#include <unordered_map>
namespace k2cache {
static void write_ktree_with_size(std::iostream &ios, K2TreeMixed &k2tree) {
  std::stringstream ss;
  auto stats = k2tree.k2tree_stats();
  auto sz = k2tree.write_to_ostream(ss);
  write_u64(ios, static_cast<uint64_t>(stats.total_bytes));
  write_u64(ios, sz);
  auto out_str = ss.str();
  ss.clear();
  auto hash = md5calc(out_str);
  ios.write(hash.data(), hash.size());
  ios.write(out_str.c_str(), sz);
}

PredicatesCacheMetadata PredicatesIndexFileBuilder::build(
    std::istream &input_file, std::ostream &output_file,
    std::iostream &tmp_stream, K2TreeConfig config) {
  FileData filedata{};
  filedata.current_triple = 0;
  filedata.size = read_u64(input_file);
  std::unique_ptr<K2TreeMixed> current_k2tree;
  std::unique_ptr<K2TreeBulkOp> current_bulk_op;
  std::vector<uint64_t> predicates_ids;
  uint64_t current_predicate = 0;
  bool first = true;

  while (!filedata.finished()) {
    auto triple = filedata.read_triple(input_file);
    if (triple.second != current_predicate || first) {
      if (current_k2tree) {
        write_ktree_with_size(tmp_stream, *current_k2tree);
      }
      current_k2tree = std::make_unique<K2TreeMixed>(
          config.treedepth, config.max_node_count, config.cut_depth);
      current_bulk_op = std::make_unique<K2TreeBulkOp>(*current_k2tree);
      current_predicate = triple.second;
      predicates_ids.push_back(current_predicate);
      first = false;
    }

    current_bulk_op->insert(triple.first, triple.third);
  }
  if (current_k2tree) {
    write_ktree_with_size(tmp_stream, *current_k2tree);
    current_k2tree.reset();
  }

  std::unordered_map<uint64_t, PredicateMetadata> metadata_map;

  uint64_t predicates_count = predicates_ids.size();
  write_u64(output_file, predicates_count);
  config.write_to_ostream(output_file);
  const auto metadata_start = output_file.tellp();
  for (const auto predicate_id : predicates_ids) {
    metadata_map[predicate_id] = PredicateMetadata{};
    metadata_map[predicate_id].write_to_ostream(output_file);
  }

  tmp_stream.seekg(0);
  int i = 0;
  for (const auto predicate_id : predicates_ids) {
    const auto total_bytes = read_u64(tmp_stream);
    const auto k2tree_serialized_size = read_u64(tmp_stream);
    const auto offset = output_file.tellp();
    std::array<char, 16> md5_read;
    tmp_stream.read(md5_read.data(), md5_read.size());
    std::vector<char> buf(k2tree_serialized_size);

    if (!tmp_stream.read(buf.data(), k2tree_serialized_size)) {
      throw std::runtime_error("Error while reading " +
                               std::to_string(k2tree_serialized_size) +
                               " bytes from tmp file");
    }

    auto md5_calc = md5calc(buf);

    if (md5_read != md5_calc) {
      throw std::runtime_error(
          "Hash between read k2tree and stored value differ at i = " +
          std::to_string(i));
    }

    output_file.write(buf.data(), k2tree_serialized_size);
    auto &tree_metadata = metadata_map[predicate_id];
    tree_metadata.predicate_id = predicate_id;
    tree_metadata.tree_offset = offset;
    tree_metadata.tree_size = k2tree_serialized_size;
    tree_metadata.tree_size_in_memory = total_bytes;
    tree_metadata.priority = 0;
    tree_metadata.k2tree_hash = std::move(md5_calc);

    i++;
  }

  const auto to_restore = output_file.tellp();
  output_file.seekp(metadata_start);
  for (const auto predicate_id : predicates_ids) {
    metadata_map[predicate_id].write_to_ostream(output_file);
  }
  output_file.seekp(to_restore);

  return PredicatesCacheMetadata(std::move(metadata_map),
                                 std::move(predicates_ids), config);
}

PredicatesCacheMetadata PredicatesIndexFileBuilder::build_debug(
    std::istream &input_file, std::ostream &output_file,
    std::iostream &tmp_stream, K2TreeConfig config, bool slow_check) {
  FileData filedata{};
  filedata.current_triple = 0;
  filedata.size = read_u64(input_file);
  std::unique_ptr<K2TreeMixed> current_k2tree;
  std::unique_ptr<K2TreeBulkOp> current_bulk_op;
  std::vector<uint64_t> predicates_ids;
  uint64_t current_predicate = 0;
  bool first = true;

  uint64_t current_counter = 0;
  std::set<uint64_t> already_added;
  while (!filedata.finished()) {
    auto triple = filedata.read_triple(input_file);
    if (triple.second != current_predicate || first) {

      if (already_added.find(triple.second) != already_added.end()) {
        throw std::runtime_error(
            "Failed at triple " + std::to_string(filedata.current_triple) +
            ". Predicate was already added: " + std::to_string(triple.second));
      }
      already_added.insert(triple.second);

      if (current_k2tree) {
        write_ktree_with_size(tmp_stream, *current_k2tree);
      }
      current_k2tree = std::make_unique<K2TreeMixed>(
          config.treedepth, config.max_node_count, config.cut_depth);
      current_bulk_op = std::make_unique<K2TreeBulkOp>(*current_k2tree);
      current_predicate = triple.second;
      predicates_ids.push_back(current_predicate);
      first = false;
      current_counter = 0;
    }

    current_bulk_op->insert(triple.first, triple.third);
    current_counter++;
    if (slow_check) {
      uint64_t scan_counter = 0;
      // This makes it very slow
      current_bulk_op->get_tree().scan_points(
          [](uint64_t, uint64_t, void *rs) {
            (*reinterpret_cast<uint64_t *>(rs))++;
          },
          &scan_counter);
      if (scan_counter != current_counter) {
        throw std::runtime_error("Points size does not match after adding (" +
                                 std::to_string(triple.first) + ", " +
                                 std::to_string(triple.second) + " ," +
                                 std::to_string(triple.third) +
                                 ") , which is at position: " +
                                 std::to_string(filedata.current_triple));
      }
    }
  }
  if (current_k2tree) {
    write_ktree_with_size(tmp_stream, *current_k2tree);
    current_k2tree.reset();
  }

  std::unordered_map<uint64_t, PredicateMetadata> metadata_map;

  uint64_t predicates_count = predicates_ids.size();
  write_u64(output_file, predicates_count);
  config.write_to_ostream(output_file);
  const auto metadata_start = output_file.tellp();
  for (const auto predicate_id : predicates_ids) {
    metadata_map[predicate_id] = PredicateMetadata{};
    metadata_map[predicate_id].write_to_ostream(output_file);
  }

  tmp_stream.seekg(0);
  int i = 0;
  for (const auto predicate_id : predicates_ids) {
    const auto total_bytes = read_u64(tmp_stream);
    const auto k2tree_serialized_size = read_u64(tmp_stream);
    const auto offset = output_file.tellp();
    std::array<char, 16> md5_read;
    tmp_stream.read(md5_read.data(), md5_read.size());
    std::vector<char> buf(k2tree_serialized_size);

    if (!tmp_stream.read(buf.data(), k2tree_serialized_size)) {
      throw std::runtime_error("Error while reading " +
                               std::to_string(k2tree_serialized_size) +
                               " bytes from tmp file");
    }

    auto md5_calc = md5calc(buf);

    if (md5_read != md5_calc) {
      throw std::runtime_error(
          "Hash between read k2tree and stored value differ at i = " +
          std::to_string(i));
    }

    output_file.write(buf.data(), k2tree_serialized_size);
    auto &tree_metadata = metadata_map[predicate_id];
    tree_metadata.predicate_id = predicate_id;
    tree_metadata.tree_offset = offset;
    tree_metadata.tree_size = k2tree_serialized_size;
    tree_metadata.tree_size_in_memory = total_bytes;
    tree_metadata.priority = 0;
    tree_metadata.k2tree_hash = std::move(md5_calc);

    i++;
  }

  const auto to_restore = output_file.tellp();
  output_file.seekp(metadata_start);
  for (const auto predicate_id : predicates_ids) {
    metadata_map[predicate_id].write_to_ostream(output_file);
  }
  output_file.seekp(to_restore);

  return PredicatesCacheMetadata(std::move(metadata_map),
                                 std::move(predicates_ids), config);
}
} // namespace k2cache