//
// Created by Cristobal Miranda, 2020
//

#include <sys/socket.h>

#include "ServerTask.hpp"

#include "responses/CacheCheckResponseMessage.hpp"
#include "responses/CacheFeedResponseMessage.hpp"
#include "responses/CacheRetrieveResponseMessage.hpp"
#include "responses/ResponseMessage.hpp"

ServerTask::ServerTask(int client_socket_fd, Message &&message, Cache &cache) :
client_socket_fd(client_socket_fd),
message(std::move(message)),
cache(cache)
{}


void send_response(int socket_client_fd, ResponseMessage *response_message){
  send(socket_client_fd, response_message->get_response_buffer(), response_message->get_response_size(), 0);
}

void process_cache_check(ServerTask &server_task){
  int client_fd = server_task.get_client_socket_fd();
  Message &message = server_task.get_message();
  auto &cache = server_task.get_cache();

  auto label = message.get_query_label();
  bool exists = cache.result_exists(label);

  auto response = CacheCheckResponseMessage(exists);

  send_response(client_fd, dynamic_cast<ResponseMessage *>(&response));
}

void process_cache_feed(ServerTask &server_task){
  int client_fd = server_task.get_client_socket_fd();
  auto &message = server_task.get_message();
  auto &cache = server_task.get_cache();

  auto feed_data = message.get_feed_data();
  auto feed_result = cache.feed(feed_data);

  auto response = CacheFeedResponseMessage(feed_result);

  send_response(client_fd, dynamic_cast<ResponseMessage *>(&response));
}

void process_cache_retrieve(ServerTask &server_task){
  int client_fd = server_task.get_client_socket_fd();
  auto &message = server_task.get_message();
  auto &cache = server_task.get_cache();

  auto label = message.get_query_label();
  bool exists = cache.result_exists(label);

  if(!exists){
    auto response = CacheCheckResponseMessage(exists);
    send_response(client_fd, dynamic_cast<ResponseMessage *>(&response));
  }
  else{
    auto &cache_graph_result = cache.get_graph_result(label);
    auto response = CacheRetrieveResponseMessage(cache_graph_result);
    send_response(client_fd, dynamic_cast<ResponseMessage *>(&response));
  }
}



void ServerTask::process() {
  switch(message.request_type()){
    case Message::ReqType::CACHE_CHECK:
      process_cache_check(*this);
      break;
    case Message::ReqType::CACHE_FEED:
      process_cache_feed(*this);
      break;

    case Message::ReqType::CACHE_RETRIEVE:
      process_cache_retrieve(*this);

    default:
      break;
  }
}

int ServerTask::get_client_socket_fd() {
  return client_socket_fd;
}

Message &ServerTask::get_message() {
  return message;
}

Cache &ServerTask::get_cache() {
  return cache;
}
