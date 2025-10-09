#include "../include/server.h"
#include "../include/client_handler.h"
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

Server::Server(int port, ThreadDispatcher &dispatcher,
               ClientManager &client_manager)
    : port(port), server_fd(-1), dispatcher(dispatcher),
      client_manager(client_manager) {}

Server::~Server() {
  if (server_fd != -1) {
    close(server_fd);
  }
}

bool Server::setup_socket() {
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    std::cerr << "nao foi possível criar o socket" << std::endl;
    return false;
  }

  // estava com problemas para testar o server por conta do erro time wait do
  // tcp, isso previne que tenha que aguardar
  int opt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    std::cerr << "falha no setsockopt" << std::endl;
    close(server_fd);
    return false;
  }

  sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    std::cerr << "falha no bind" << std::endl;
    close(server_fd);
    return false;
  }

  if (listen(server_fd, 10) < 0) {
    std::cerr << "falha no listen" << std::endl;
    close(server_fd);
    return false;
  }
  return true;
}

void Server::start() {
  if (!setup_socket()) {
    return;
  }

  std::cout << "servidor na porta " << port << std::endl;

  while (true) {
    int client_socket = accept(server_fd, nullptr, nullptr);
    if (client_socket < 0) {
      std::cerr << "falha ao aceitar a conexão do cliente" << std::endl;
      continue;
    }

    std::cout << "client conectado no socket: " << client_socket << std::endl;
    client_manager.add(client_socket);

    // enfileira a tarefa para o dispatcher em vez de criar uma nova thread
    dispatcher.enqueue([client_socket, this]() {
      handle_client(client_socket, this->client_manager);
    });
  }
}
