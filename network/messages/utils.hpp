//
// Created by cristobal on 18-05-22.
//

#ifndef RDFCACHEK2_UTILS_HPP
#define RDFCACHEK2_UTILS_HPP

#include <cstddef>
bool read_nbytes_from_socket(int client_socket_fd, char *read_buffer,
                             size_t bytes_to_read);

#endif // RDFCACHEK2_UTILS_HPP
