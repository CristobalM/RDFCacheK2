//
// Created by Cristobal Miranda, 2020
//

#include "ServerTask.hpp"

ServerTask::ServerTask(int client_socket_fd, std::unique_ptr<char[]> &&message) :
client_socket_fd(client_socket_fd), message(std::move(message)) {
}
