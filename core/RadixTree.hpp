//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_RADIXTREE_HPP
#define RDFCACHEK2_RADIXTREE_HPP

#include <memory>

extern "C" {
#include <rax.h>
}

#include "NoDataEntity.hpp"
#include "serialization_util.hpp"
#include <map>
#include <netinet/in.h>
#include <string>
#include <vector>

#include <radix_tree.pb.h>

const unsigned long DEFAULT_FLUSH_AMOUNT = 1'000'000;

template <class DataT = NoDataEntity> class RadixTree {
  rax *inner_rt;
  std::vector<std::unique_ptr<DataT>> data_holder;
  unsigned long flush_amount = DEFAULT_FLUSH_AMOUNT;

  unsigned long serialized_nodes = 0;

public:
  class LookupResult {
    bool _was_found;
    DataT *_result;

  public:
    LookupResult(bool _was_found, DataT *_result)
        : _was_found(_was_found), _result(_result) {}

    bool was_found() { return _was_found; }

    DataT &result() {
      if (!was_found()) {
        throw std::runtime_error("Result was not found");
      }
      return *_result;
    }
  };

  RadixTree() : inner_rt(raxNew()) {}

  ~RadixTree() { raxFree(inner_rt); }

  RadixTree(const RadixTree &) = delete;

  RadixTree(RadixTree &&rhs)
      : inner_rt(rhs.inner_rt), data_holder(std::move(rhs.data_holder)) {}

  template <typename DataU = DataT>
  typename std::enable_if<std::is_same<DataU, NoDataEntity>::value != 0,
                          void>::type
  insert(const std::string &key) {
    raxInsert(
        inner_rt,
        reinterpret_cast<unsigned char *>(const_cast<char *>(key.c_str())),
        key.size(), nullptr, nullptr);
  }

  template <typename DataU = DataT>
  typename std::enable_if<std::is_same<DataU, NoDataEntity>::value == 0,
                          void>::type
  insert(const std::string &key, DataT &value) {
    auto d_data = std::make_unique<DataT>(value);
    auto *held_ptr = d_data.get();
    data_holder.push_back(std::move(d_data));
    raxInsert(
        inner_rt,
        reinterpret_cast<unsigned char *>(const_cast<char *>(key.c_str())),
        key.size(), reinterpret_cast<void *>(held_ptr), nullptr);
  }

  LookupResult lookup(const std::string &key) {
    void *v_data = raxFind(
        inner_rt,
        reinterpret_cast<unsigned char *>(const_cast<char *>(key.c_str())),
        key.size());

    if (v_data == raxNotFound) {
      return LookupResult(false, nullptr);
    }

    return LookupResult(true, reinterpret_cast<DataT *>(v_data));
  }

  void remove(const std::string &key) {
    raxRemove(
        inner_rt,
        reinterpret_cast<unsigned char *>(const_cast<char *>(key.c_str())),
        key.size(), nullptr);
  }

  rax *get_inner_rt() { return inner_rt; }

  DataT *add_data(DataT &data) {
    auto sptr = std::make_unique<DataT>(data);
    auto *raw_ptr = sptr.get();
    data_holder.push_back(std::move(sptr));
    return raw_ptr;
  }

  DataT *add_data(std::unique_ptr<DataT> &&data) {
    auto *raw_ptr = data.get();
    data_holder.push_back(std::move(data));
    return raw_ptr;
  }

  void _debug_print_radix_tree() { raxShow(inner_rt); }

  void deserialize(std::istream &input_stream) {
    free(inner_rt->head);

    std::map<uint32_t, raxNode *> deserialized;

    auto header = read_header(input_stream);

    std::cout << "protos_count read: " << header.protos_count << std::endl;
    std::cout << "max_size read: " << header.max_size << std::endl;

    std::vector<char> buffer(header.max_size, 0);
    char *buffer_ptr = buffer.data();

    inner_rt->numele = header.numele;
    inner_rt->numnodes = header.num_nodes;

    uint32_t proto_size;
    for (uint32_t i = 0; i < header.protos_count; i++) {
      input_stream.read(reinterpret_cast<char *>(&proto_size),
                        sizeof(uint32_t));
      proto_size = ntohl(proto_size);
      input_stream.read(buffer_ptr, proto_size);

      proto_msg::RadixNode radix_node;
      radix_node.ParseFromArray(buffer_ptr, proto_size);

      DeserializedResult deserialized_result =
          deserialize_node(radix_node, deserialized);

      if (deserialized_result.child_id == header.root_index) {
        inner_rt->head = deserialized_result.deserialized_node;
      }
    }
  }

  void set_serialization_flush_amount(unsigned long amount) {
    flush_amount = amount;
  }

  void serialize(std::ostream &output_stream) {
    serialized_nodes = 0;

    HeaderHolder header{};
    header.max_size = 0;     // to fill later
    header.protos_count = 0; // to fill later
    header.root_index = 0;   // to fill later
    header.numele = inner_rt->numele;
    header.num_nodes = inner_rt->numnodes;

    std::map<uint32_t, std::unique_ptr<proto_msg::RadixNode>> to_serialize;

    // Place holder to fix after recursive serialization
    auto starting_byte = output_stream.tellp();
    write_header(header, output_stream);

    auto root_node = std::make_unique<proto_msg::RadixNode>();
    header.root_index =
        serialize_node(root_node.get(), inner_rt->head, to_serialize,
                       header.protos_count, output_stream, header.max_size);
    to_serialize[header.root_index] = std::move(root_node);

    std::cout << "protos count write: " << header.protos_count << std::endl;
    std::cout << "protos max_size write: " << header.max_size << std::endl;

    // Fix placeholder
    output_stream.seekp(starting_byte);
    write_header(header, output_stream);
  }

  uint32_t serialize_node(
      proto_msg::RadixNode *proto_node, raxNode *rax_node,
      std::map<uint32_t, std::unique_ptr<proto_msg::RadixNode>> &to_serialize,
      uint32_t &node_counter, std::ostream &output_stream, uint32_t &max_size) {
    proto_node->set_is_key(rax_node->iskey);
    proto_node->set_is_null(rax_node->isnull);
    proto_node->set_is_compr(rax_node->iscompr);
    proto_node->set_size(rax_node->size);
    uint32_t node_id = node_counter++;
    proto_node->set_node_id(node_id);

    if (!rax_node->isnull) {
      if (rax_node->iskey) {
        if constexpr (!std::is_same<DataT, NoDataEntity>::value) {
          auto *data = reinterpret_cast<DataT *>(raxGetData(rax_node));
          proto_node->set_data(data->to_bytes_string());
        }
      }

      if (rax_node->iscompr) {
        proto_node->mutable_compr_node()->set_compressed_data(rax_node->data,
                                                              rax_node->size);
        auto child_node = std::make_unique<proto_msg::RadixNode>();
        auto child_id =
            serialize_node(child_node.get(), *raxNodeFirstChildPtr(rax_node),
                           to_serialize, node_counter, output_stream, max_size);
        to_serialize[child_id] = std::move(child_node);
        proto_node->mutable_compr_node()->set_child_id(child_id);

      } else {
        proto_node->mutable_normal_node()->set_children_chars(rax_node->data,
                                                              rax_node->size);
        for (int i = 0; i < rax_node->size; i++) {
          auto child_node = std::make_unique<proto_msg::RadixNode>();
          auto child_id = serialize_node(
              child_node.get(), *(raxNodeFirstChildPtr(rax_node) + i),
              to_serialize, node_counter, output_stream, max_size);
          to_serialize[child_id] = std::move(child_node);

          proto_node->mutable_normal_node()->add_children_ids(child_id);
        }
      }
    }

    auto serialized_string = proto_node->SerializeAsString();
    auto serialized_size =
        static_cast<uint32_t>(htonl(serialized_string.size()));
    output_stream.write(reinterpret_cast<const char *>(&serialized_size),
                        sizeof(uint32_t));
    output_stream.write(serialized_string.c_str(), serialized_string.size());

    to_serialize.erase(node_id);

    max_size =
        std::max(max_size, static_cast<uint32_t>(serialized_string.size()));

    serialized_nodes++;

    if (serialized_nodes % flush_amount == 0) {
      output_stream.flush();
    }

    return node_id;
  }

private:
  struct HeaderHolder {
    uint32_t max_size;
    uint32_t protos_count;
    uint32_t root_index;
    uint32_t numele;
    uint32_t num_nodes;
  };

  struct DeserializedResult {
    raxNode *deserialized_node;
    uint32_t child_id;
  };

  HeaderHolder read_header(std::istream &input_stream) {
    HeaderHolder output{};
    output.max_size = read_u32(input_stream);
    output.protos_count = read_u32(input_stream);
    output.root_index = read_u32(input_stream);
    output.numele = read_u32(input_stream);
    output.num_nodes = read_u32(input_stream);
    return output;
  }

  void write_header(HeaderHolder header_holder, std::ostream &output_stream) {
    write_u32(output_stream, header_holder.max_size);
    write_u32(output_stream, header_holder.protos_count);
    write_u32(output_stream, header_holder.root_index);
    write_u32(output_stream, header_holder.numele);
    write_u32(output_stream, header_holder.num_nodes);
  }

  DeserializedResult
  deserialize_node(const proto_msg::RadixNode &proto_node,
                   std::map<uint32_t, raxNode *> &deserialized) {

    raxNode *new_node;
    size_t node_size;

    if (proto_node.has_compr_node()) {
      node_size = sizeof(raxNode) + proto_node.size() +
                  raxPadding(proto_node.size()) + sizeof(raxNode *);
      new_node = reinterpret_cast<raxNode *>(malloc(node_size));

      new_node->isnull = proto_node.is_null();
      new_node->iscompr = proto_node.is_compr();
      new_node->iskey = proto_node.is_key();
      new_node->size = proto_node.size();

      memcpy(new_node->data, proto_node.compr_node().compressed_data().c_str(),
             proto_node.size());

      raxNode *child_node = deserialized[proto_node.compr_node().child_id()];

      memcpy(raxNodeFirstChildPtr(new_node), &child_node, sizeof(raxNode *));

    } else if (proto_node.has_normal_node()) {
      node_size = sizeof(raxNode) + proto_node.size() +
                  raxPadding(proto_node.size()) +
                  sizeof(raxNode *) * proto_node.size();

      if (!proto_node.is_null()) {
        node_size += sizeof(void *);
      }

      new_node = reinterpret_cast<raxNode *>(malloc(node_size));
      new_node->isnull = proto_node.is_null();
      new_node->iscompr = proto_node.is_compr();
      new_node->iskey = proto_node.is_key();
      new_node->size = proto_node.size();

      for (int i = 0; i < proto_node.normal_node().children_ids_size(); i++) {

        new_node->data[i] = proto_node.normal_node().children_chars()[i];

        raxNode *child = deserialized[proto_node.normal_node().children_ids(i)];

        memcpy(raxNodeFirstChildPtr(new_node) + i, &child, sizeof(raxNode *));
      }

    } else {
      throw std::runtime_error("Unknown node type");
    }

    if (!proto_node.data().empty()) {
      auto *data = add_data(DataT::create_from_bytes_string(proto_node.data()));
      raxSetData(new_node, data);
    }

    deserialized[proto_node.node_id()] = new_node;

    DeserializedResult result;
    result.child_id = proto_node.node_id();
    result.deserialized_node = new_node;

    return result;
  }

  constexpr size_t raxPadding(size_t nodesize) {
    return ((sizeof(void *) - ((nodesize + 4) % sizeof(void *))) &
            (sizeof(void *) - 1));
  }

  constexpr raxNode **raxNodeFirstChildPtr(raxNode *n) {
    return ((raxNode **)((n)->data + (n)->size + raxPadding((n)->size)));
  }
};

#endif // RDFCACHEK2_RADIXTREE_HPP
