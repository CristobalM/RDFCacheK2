//
// Created by cristobal on 18-05-22.
//

#ifndef RDFCACHEK2_UTILS_HPP
#define RDFCACHEK2_UTILS_HPP

#include "nodeids/TripleNodeId.hpp"
#include "sparql_tree.pb.h"
#include <cstddef>
namespace k2cache {
bool read_nbytes_from_socket(int client_socket_fd, char *read_buffer,
                             size_t bytes_to_read);
TripleNodeId
proto_triple_to_internal(const proto_msg::TripleNodeIdEnc &proto_triple);
} // namespace k2cache
#endif // RDFCACHEK2_UTILS_HPP
