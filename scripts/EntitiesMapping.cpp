//
// Created by cristobal on 30-05-20.
//

#include <exception>
#include <fstream>
#include <netinet/in.h>
#include <string>

#include <stdlib.h>

#include "EntitiesMapping.h"

#define raxPadding(nodesize)                                                   \
  ((sizeof(void *) - ((nodesize + 4) % sizeof(void *))) & (sizeof(void *) - 1))

/* Return the pointer to the first child pointer. */
#define raxNodeFirstChildPtr(n)                                                \
  ((raxNode **)((n)->data + (n)->size + raxPadding((n)->size)))

EntitiesMapping::EntitiesMapping(std::istream &input_stream) {
  deserialize(input_stream);
}

unsigned long EntitiesMapping::add_subject(const std::string &value,
                                           Entity::EntityType entity_type) {

  auto lookup_result = entities_mapping.lookup(value);

  if (lookup_result.was_found()) {
    if (!lookup_result.result().is_subject()) {
      lookup_result.result().mark_as_subject();
      lookup_result.result().subject_value = subjects_count++;
    }
    return lookup_result.result().subject_value;
  }
  auto mapped_value = subjects_count++;
  Entity saving_entity{};
  saving_entity.subject_value = mapped_value;
  saving_entity.entity_type = entity_type;
  saving_entity.mark_as_subject();
  entities_mapping.insert(value, saving_entity);
  return mapped_value;
}

unsigned long EntitiesMapping::add_predicate(const std::string &value,
                                             Entity::EntityType entity_type) {
  auto lookup_result = entities_mapping.lookup(value);

  if (lookup_result.was_found()) {
    if (!lookup_result.result().is_predicate()) {
      lookup_result.result().mark_as_predicate();
      lookup_result.result().predicate_value = predicates_count++;
    }
    return lookup_result.result().predicate_value;
  }
  auto mapped_value = predicates_count++;
  Entity saving_entity{};
  saving_entity.predicate_value = mapped_value;
  saving_entity.entity_type = entity_type;
  saving_entity.mark_as_predicate();
  entities_mapping.insert(value, saving_entity);
  return mapped_value;
}

unsigned long EntitiesMapping::add_object(const std::string &value,
                                          Entity::EntityType entity_type) {
  auto lookup_result = entities_mapping.lookup(value);

  if (lookup_result.was_found()) {
    if (!lookup_result.result().is_object()) {
      lookup_result.result().mark_as_object();
      lookup_result.result().object_value = objects_count++;
    }
    return lookup_result.result().object_value;
  }
  auto mapped_value = objects_count++;
  Entity saving_entity{};
  saving_entity.object_value = mapped_value;
  saving_entity.entity_type = entity_type;
  saving_entity.mark_as_object();
  entities_mapping.insert(value, saving_entity);
  return mapped_value;
}

bool EntitiesMapping::has_subject(const std::string &value) {
  auto lookup_result = entities_mapping.lookup(value);
  return lookup_result.was_found() && lookup_result.result().is_subject();
}

bool EntitiesMapping::has_predicate(const std::string &value) {
  auto lookup_result = entities_mapping.lookup(value);
  return lookup_result.was_found() && lookup_result.result().is_predicate();
}

bool EntitiesMapping::has_object(const std::string &value) {
  auto lookup_result = entities_mapping.lookup(value);
  return lookup_result.was_found() && lookup_result.result().is_object();
}

EntitiesMapping::EntitiesMapping()
    : subjects_count(0), predicates_count(0), objects_count(0) {}

void write_u32(std::ostream &output_stream, uint32_t value) {
  value = htonl(value);
  output_stream.write(reinterpret_cast<char *>(&value), sizeof(uint32_t));
}

uint32_t read_u32(std::istream &input_stream) {
  uint32_t result;
  input_stream.read(reinterpret_cast<char *>(&result), sizeof(uint32_t));
  result = ntohl(result);
  return result;
}

struct HeaderHolder {
  uint32_t subjects_count;
  uint32_t predicates_count;
  uint32_t objects_count;
  uint32_t max_size;
  uint32_t protos_count;
  uint32_t root_index;
  uint32_t numele;
  uint32_t num_nodes;
};

void write_header(HeaderHolder header_holder, std::ostream &output_stream) {
  write_u32(output_stream, header_holder.subjects_count);
  write_u32(output_stream, header_holder.predicates_count);
  write_u32(output_stream, header_holder.objects_count);
  write_u32(output_stream, header_holder.max_size);
  write_u32(output_stream, header_holder.protos_count);
  write_u32(output_stream, header_holder.root_index);
  write_u32(output_stream, header_holder.numele);
  write_u32(output_stream, header_holder.num_nodes);
}

HeaderHolder read_header(std::istream &input_stream) {
  HeaderHolder output{};
  output.subjects_count = read_u32(input_stream);
  output.predicates_count = read_u32(input_stream);
  output.objects_count = read_u32(input_stream);
  output.max_size = read_u32(input_stream);
  output.protos_count = read_u32(input_stream);
  output.root_index = read_u32(input_stream);
  output.numele = read_u32(input_stream);
  output.num_nodes = read_u32(input_stream);
  return output;
}

void EntitiesMapping::serialize(std::ostream &output_stream) {

  rax *inner_rt = entities_mapping.get_inner_rt();

  HeaderHolder header{};
  header.subjects_count = subjects_count;
  header.predicates_count = predicates_count;
  header.objects_count = objects_count;
  header.max_size = 0;     // to fill later
  header.protos_count = 0; // to fill later
  header.root_index = 0;   // to fill later
  header.numele = inner_rt->numele;
  header.num_nodes = inner_rt->numnodes;

  std::map<uint32_t, std::unique_ptr<proto_msg::RadixNode>> to_serialize;

  // Place holder to fix after recursive serialization
  write_header(header, output_stream);

  auto root_node = std::make_unique<proto_msg::RadixNode>();
  header.root_index =
      serialize_node(root_node.get(), inner_rt->head, to_serialize,
                     header.protos_count, output_stream, header.max_size);
  to_serialize[header.root_index] = std::move(root_node);

  std::cout << "protos count write: " << header.protos_count << std::endl;
  std::cout << "protos max_size write: " << header.max_size << std::endl;

  // Fix placeholder
  output_stream.seekp(0);
  write_header(header, output_stream);
}

void clear_subtree(
    std::map<uint32_t, std::unique_ptr<proto_msg::RadixNode>> &to_serialize,
    uint32_t root_id) {
  if (to_serialize.find(root_id) == to_serialize.end()) {
    return;
  }
  auto root_node = std::make_unique<proto_msg::RadixNode>();

  auto &root = to_serialize[root_id];
  if (root == nullptr) {
    return;
  }
  if (root->size() == 0) {
    to_serialize.erase(root_id);
    return;
  }

  if (root->is_compr()) {
    clear_subtree(to_serialize, root->compr_node().child_id());
  } else {
    for (int i = 0; i < root->normal_node().children_ids_size(); i++) {
      clear_subtree(to_serialize, root->normal_node().children_ids(i));
    }
  }
  to_serialize.erase(root_id);
}

uint32_t EntitiesMapping::serialize_node(
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
      auto *entity = reinterpret_cast<Entity *>(raxGetData(rax_node));
      uint8_t both_type_kind = 0;
      both_type_kind = static_cast<uint8_t>(entity->entity_type) << 3u;
      both_type_kind |= entity->entity_kinds;
      proto_node->mutable_entity()->set_entity_type_kind(
          &both_type_kind, sizeof(both_type_kind));
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
  auto serialized_size = static_cast<uint32_t>(htonl(serialized_string.size()));
  output_stream.write(reinterpret_cast<const char *>(&serialized_size),
                      sizeof(uint32_t));
  output_stream.write(serialized_string.c_str(), serialized_string.size());

  // clear_subtree(to_serialize, node_id);

  max_size =
      std::max(max_size, static_cast<uint32_t>(serialized_string.size()));

  return node_id;
}

void EntitiesMapping::deserialize(std::istream &input_stream) {
  rax *inner_rt = entities_mapping.get_inner_rt();
  free(inner_rt->head);

  std::map<uint32_t, raxNode *> deserialized;

  auto header = read_header(input_stream);

  std::cout << "protos_count read: " << header.protos_count << std::endl;
  std::cout << "max_size read: " << header.max_size << std::endl;

  std::vector<char> buffer(header.max_size, 0);
  char *buffer_ptr = buffer.data();

  subjects_count = header.subjects_count;
  objects_count = header.objects_count;
  predicates_count = header.predicates_count;

  inner_rt->numele = header.numele;
  inner_rt->numnodes = header.num_nodes;

  uint32_t proto_size;
  for (uint32_t i = 0; i < header.protos_count; i++) {
    input_stream.read(reinterpret_cast<char *>(&proto_size), sizeof(uint32_t));
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

EntitiesMapping::DeserializedResult
EntitiesMapping::deserialize_node(const proto_msg::RadixNode &proto_node,
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

  if (proto_node.has_entity()) {
    Entity entity{};

    auto both_type_kind =
        static_cast<uint8_t>(proto_node.entity().entity_type_kind()[0]);

    entity.entity_kinds = both_type_kind & 7u;
    entity.entity_type = static_cast<Entity::EntityType>(both_type_kind >> 3u);
    entity.subject_value = proto_node.entity().subject_value();
    entity.predicate_value = proto_node.entity().predicate_value();
    entity.object_value = proto_node.entity().object_value();
    Entity *entity_stored = entities_mapping.add_data(entity);

    raxSetData(new_node, entity_stored);
  }

  deserialized[proto_node.node_id()] = new_node;

  DeserializedResult result;
  result.child_id = proto_node.node_id();
  result.deserialized_node = new_node;

  return result;
}

EntitiesMapping::EntitiesMapping(EntitiesMapping &&rhs)
    : entities_mapping(std::move(rhs.entities_mapping)),
      subjects_count(rhs.subjects_count),
      predicates_count(rhs.predicates_count), objects_count(rhs.objects_count) {
}

void EntitiesMapping::_debug_print_radix_tree() {
  entities_mapping._debug_print_radix_tree();
}

std::shared_ptr<EntitiesMapping>
EntitiesMapping::load_from_file(const std::string &previous_mapping_fpath) {
  std::ifstream ifs(previous_mapping_fpath, std::ios::binary);
  return std::make_shared<EntitiesMapping>(ifs);
}
