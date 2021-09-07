//
// Created by cristobal on 9/6/21.
//

#include <K2TreeBulkOp.hpp>
#include <UpdatesLogger.hpp>
#include <gtest/gtest.h>

struct DataMergerMock : public I_DataMerger {

  K2TreeConfig config;
  explicit DataMergerMock(K2TreeConfig config) : config(config) {}

  NaiveDynamicStringDictionary main_dict;

  std::unordered_map<unsigned long, std::unique_ptr<K2TreeMixed>> trees;

  void merge_with_extra_dict(
      NaiveDynamicStringDictionary &input_extra_dict) override {
    main_dict.merge_with_extra_dict(input_extra_dict);
  }
  void merge_add_tree(unsigned long predicate_id,
                      K2TreeMixed &k2tree) override {
    auto it = trees.find(predicate_id);
    K2TreeMixed *tree_ptr;
    if (it == trees.end()) {
      auto new_tree = std::make_unique<K2TreeMixed>(config);
      tree_ptr = new_tree.get();
      trees[predicate_id] = std::move(new_tree);
    } else {
      tree_ptr = it->second.get();
    }

    auto scanner = k2tree.create_full_scanner();
    while (scanner->has_next()) {
      auto point = scanner->next();
      tree_ptr->insert(point.first, point.second);
    }
  }
  void merge_delete_tree(unsigned long predicate_id,
                         K2TreeMixed &k2tree) override {
    auto it = trees.find(predicate_id);
    K2TreeMixed *tree_ptr;
    if (it == trees.end()) {
      auto new_tree = std::make_unique<K2TreeMixed>(config);
      tree_ptr = new_tree.get();
      trees[predicate_id] = std::move(new_tree);
    } else {
      tree_ptr = it->second.get();
    }

    auto scanner = k2tree.create_full_scanner();
    while (scanner->has_next()) {
      auto point = scanner->next();
      tree_ptr->remove(point.first, point.second);
    }
  }

  void drop() {
    main_dict = NaiveDynamicStringDictionary();
    trees = std::unordered_map<unsigned long, std::unique_ptr<K2TreeMixed>>();
  }
  void merge_update(std::vector<K2TreeUpdates> &updates) override {
    for (auto &update : updates) {
      if (update.k2tree_add)
        merge_add_tree(update.predicate_id, *update.k2tree_add);
      if (update.k2tree_del)
        merge_delete_tree(update.predicate_id, *update.k2tree_del);
    }
  }
};

struct StringIStream : public I_IStream {
  std::string data;
  std::istringstream iss;

public:
  StringIStream(std::string &data, std::ios::openmode openmode)
      : data(data), iss(data, openmode) {}
  void seekg(std::streamoff offset, std::ios_base::seekdir way) override {
    iss.seekg(offset, way);
  }
  explicit operator bool() const override { return iss.operator bool() && !iss.eof() && iss.good(); }
  std::istream &get_stream() override { return iss; }
};

struct StringOStream : public I_OStream {
  std::string &data;
  std::ostringstream oss;

public:
  StringOStream(std::string &data, std::ios::openmode openmode)
      : data(data), oss(openmode) {}

  ~StringOStream() { internal_flush(); }
  void flush() override { internal_flush(); }
  std::ostream &get_stream() override { return oss; }

private:
  void internal_flush() { data = oss.str(); }
};

struct FHMock : public I_FileRWHandler {
  std::string &data;
  std::string &temp_data;
  FHMock(std::string &data, std::string &temp_data)
      : data(data), temp_data(temp_data) {}

  std::unique_ptr<I_OStream> get_writer(std::ios::openmode) override {
    return std::make_unique<StringOStream>(data, std::ios::out | std::ios::app |
                                                     std::ios::binary);
  }
  std::unique_ptr<I_IStream> get_reader(std::ios::openmode) override {
    return std::make_unique<StringIStream>(data,
                                           std::ios::in | std::ios::binary);
  }
  bool exists() override { return true; }
  std::unique_ptr<I_OStream> get_writer_temp(std::ios::openmode) override {
    return std::make_unique<StringOStream>(
        temp_data, std::ios::out | std::ios::trunc | std::ios::binary);
  }
  void commit_temp_writer() override { data = temp_data; }
};

TEST(update_log_test, test_1) {
  K2TreeConfig config;
  config.treedepth = 32;
  config.cut_depth = 10;
  config.max_node_count = 256;
  DataMergerMock data_merger(config);
  std::string data;
  std::string temp_data;
  std::string data_offsets;
  std::string temp_data_offsets;
  FHMock fh(data, temp_data);
  FHMock fh_offsets(data_offsets, temp_data_offsets);
  UpdatesLogger updates_logger(data_merger, fh, fh_offsets);

  NaiveDynamicStringDictionary sd;
  int size = 10;
  int predicate_id_1 = 123;
  for (int i = 0; i < size; i++) {
    sd.add_resource(RDFResource("Resource #" + std::to_string(i),
                                RDFResourceType::RDF_TYPE_LITERAL));
  }
  K2TreeMixed k2tree(config);
  K2TreeBulkOp op(k2tree);
  int size_tree = 10000;
  for (int i = 0; i < size_tree; i++) {
    op.insert(i + 1, i + 1);
  }
  K2TreeUpdates tree_update_1(predicate_id_1, &k2tree, nullptr);
  std::vector<K2TreeUpdates> tree_updates = {tree_update_1};
  updates_logger.log(&sd, tree_updates);

  ASSERT_TRUE(updates_logger.has_predicate_stored(predicate_id_1));
  ASSERT_EQ(data_merger.main_dict.size(), size);
  ASSERT_EQ(k2tree.size(), size_tree);
  data_merger.drop();
  ASSERT_EQ(data_merger.main_dict.size(), 0);
  ASSERT_EQ(data_merger.trees.size(), 0);
  updates_logger.recover_all();
  ASSERT_EQ(data_merger.main_dict.size(), sd.size());
  ASSERT_GT(data_merger.main_dict.size(), 0);
  ASSERT_EQ(data_merger.trees.size(), 1);

  auto &dm_tree = *data_merger.trees[predicate_id_1];

  ASSERT_EQ(dm_tree.size(), k2tree.size());
}

TEST(oss_test, test1) {
  std::string s;
  StringOStream oss(s, std::ios::binary | std::ios::out | std::ios::app);
  oss.get_stream() << "hola que tal"
                   << " bueno" << std::endl;
  oss.flush();
  std::cout << "s is: " << s << std::endl;
}