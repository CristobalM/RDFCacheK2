//
// Created by Cristobal Miranda, 2020
//

#include "TCPServerConnection.hpp"

#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <MemoryManager.hpp>

#include "exception/CantEstablishSocketException.hpp"
#include "exception/CantBindToPortException.hpp"
#include "exception/CantStartListeningException.hpp"

template <class TCPServerTaskProcessor>
TCPServerConnection<TCPServerTaskProcessor>::TCPServerConnection(uint16_t port)
: port(port){
}



template <class TCPServerTaskProcessor>
void TCPServerConnection<TCPServerTaskProcessor>::start() {
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(server_fd == -1){
    throw CantEstablishSocketException(port);
  }

  int client_socket_fd;


  struct sockaddr_in address{};

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);
  socklen_t addrlen = sizeof(address);


  if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
    throw CantBindToPortException(port);
  }

  if(listen(server_fd, 3) < 0){
    throw CantStartListeningException(port);
  }


  uint32_t msg_size;


  for(;;){
    client_socket_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if(client_socket_fd < 0){
      // Error while accepting
      std::cerr << "Error while accepting connection" << std::endl;
      continue;
    }

    int val_read = read(client_socket_fd, (char *)&msg_size, sizeof(msg_size));

    if(val_read < 0){
      std::cerr << "Error while reading msg_size data from connection" << std::endl;
      continue;
    }

    auto buffer = std::make_unique<char[]>(msg_size);

    val_read = read(client_socket_fd, buffer.get(), msg_size * sizeof(char));

    if(val_read < 0){
      std::cerr << "Error while reading data from connection" << std::endl;
      continue;
    }


    TCPServerTaskProcessor::process_request(client_socket_fd, std::move(buffer));
  }

}
