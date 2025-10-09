#pragma once

#include "client_manager.h"
#include "thread_dispatcher.h"

class Server {
public:
  Server(int port, ThreadDispatcher &dispatcher, ClientManager &client_manager);
  ~Server();

  void start();

private:
  int port;
  int server_fd; // fd = file descriptor, socket listener
  ThreadDispatcher &dispatcher;
  ClientManager &client_manager;

  // pattern pra n ter que ficar criando coisa todas as vezes
  bool setup_socket();
};
