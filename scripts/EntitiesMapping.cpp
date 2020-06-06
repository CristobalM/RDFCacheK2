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

EntitiesMapping::EntitiesMapping(proto_msg::EntitiesMapping &input_proto) {
  deserialize_tree(*input_proto.mutable_entities_mapping());
  subjects_count = input_proto.subjects_count();
  predicates_count = input_proto.predicates_count();
  objects_count = input_proto.objects_count();
}

std::unique_ptr<proto_msg::EntitiesMapping> EntitiesMapping::serialize() {
  auto out = std::make_unique<proto_msg::EntitiesMapping>();
  proto_msg::RadixTree *radix_tree = out->mutable_entities_mapping();
  serialize_tree(*radix_tree);
  return out;
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

void EntitiesMapping::serialize_tree(proto_msg::RadixTree &radix_tree,
                                     std::ostream &output_stream) {

  rax *inner_rt = entities_mapping.get_inner_rt();

  radix_tree.set_numele(inner_rt->numele);
  radix_tree.set_num_nodes(inner_rt->numnodes);
  auto root_node = std::make_unique<proto_msg::RadixNode>();
  auto *root_node_raw = root_node.get();
  std::map<uint32_t, std::unique_ptr<proto_msg::RadixNode>> to_serialize;
  uint32_t node_counter = 0;
  to_serialize[node_counter] = std::move(root_node);

  uint32_t max_size = 0;
  // Placeholder to fill with the calculated max_size later
  output_stream.write(reinterpret_cast<char *>(&max_size), sizeof(uint32_t));


  auto header_str = radix_tree.SerializeAsString();
  auto header_sz = htonl(header_str.size());

  max_size = std::max(max_size, static_cast<uint32_t>(header_str.size()));

  output_stream.write(reinterpret_cast<char *>(&header_sz), sizeof(uint32_t));
  output_stream.write(header_str.data(), header_str.size());

  serialize_node(root_node_raw, inner_rt->head, to_serialize, node_counter,
                 output_stream, max_size);

  output_stream.seekp(0);
  max_size = htonl(max_size);
  output_stream.write(reinterpret_cast<const char *>(&max_size), sizeof(uint32_t));
}

void clear_subtree(
    std::map<uint32_t, std::unique_ptr<proto_msg::RadixNode>> &to_serialize,
    uint32_t root_id) {
  auto &root = to_serialize[root_id];
  if (root->size() == 0) {
    to_serialize.erase(root_id);
    return;
  }

  if (root->is_compr()) {
    clear_subtree(to_serialize, root->compr_node().child_id());
  } else {
    for (auto i = 0u; i < root->normal_node().children_ids_size(); i++) {
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
      auto *child_node_raw = child_node.get();
      to_serialize[node_id] = std::move(child_node);
      auto child_id =
          serialize_node(child_node_raw, *raxNodeFirstChildPtr(rax_node),
                         to_serialize, node_counter, output_stream);
      proto_node->mutable_compr_node()->set_child_id(child_id);

    } else {
      proto_node->mutable_normal_node()->set_children_chars(rax_node->data,
                                                            rax_node->size);
      for (int i = 0; i < rax_node->size; i++) {
        auto child_node = std::make_unique<proto_msg::RadixNode>();
        auto *child_node_raw = child_node.get();
        to_serialize[node_id] = std::move(child_node);
        auto child_id = serialize_node(
            child_node_raw, *(raxNodeFirstChildPtr(rax_node) + i), to_serialize,
            node_counter, output_stream);
        proto_node->mutable_normal_node()->add_children_ids(child_id);
      }
    }
  }

  auto serialized_string = proto_node->SerializeAsString();
  auto serialized_size = static_cast<uint32_t>(htonl(serialized_string.size()));
  output_stream.write(reinterpret_cast<const char *>(&serialized_size),
                      sizeof(uint32_t));
  output_stream.write(serialized_string.c_str(), serialized_string.size());

  clear_subtree(to_serialize, node_id);

  max_size = std::max(max_size, static_cast<uint32_t>(serialized_string.size()));

  return node_id;
}

void EntitiesMapping::deserialize_tree(proto_msg::RadixTree &proto_radix_tree,
                                       std::istream &input_stream) {
  rax *inner_rt = entities_mapping.get_inner_rt();
  inner_rt->numele = proto_radix_tree.numele();
  inner_rt->numnodes = proto_radix_tree.num_nodes();
  std::map<uint32_t, std::unique_ptr<proto_msg::RadixNode>> to_deserialize;

  free(inner_rt->head);

  uint32_t max_size;
  input_stream.read(reinterpret_cast<char *>(&max_size), sizeof(uint32_t));
  max_size = ntohl(max_size);

  std::vector<char> buffer(max_size, 0);
  char *buffer_ptr = buffer.data();

  for(;;){
    uint32_t proto_size;
    input_stream.read(reinterpret_cast<char *>(&proto_size), sizeof(uint32_t));
    proto_size = ntohl(proto_size);

    input_stream.read(buffer_ptr, proto_size);



  }


  //inner_rt->head = deserialize_node(inner_rt, *root, to_deserialize, input_stream);
}

raxNode *EntitiesMapping::deserialize_node(
    rax *rax_tree, const proto_msg::RadixNode &proto_node,
    std::map<uint32_t, std::unique_ptr<proto_msg::RadixNode>> &to_deserialize,
    std::istream &input_stream) {

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

    raxNode *child_node =
        deserialize_node(rax_tree, proto_node.compr_node().child());

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

    for (int i = 0; i < proto_node.normal_node().children_size(); i++) {

      new_node->data[i] = proto_node.normal_node().children_chars()[i];

      raxNode *child =
          deserialize_node(rax_tree, proto_node.normal_node().children(i));
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

  return new_node;
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
  proto_msg::EntitiesMapping proto_saved;
  proto_saved.ParseFromIstream(&ifs);
  return std::make_shared<EntitiesMapping>(proto_saved);
}
