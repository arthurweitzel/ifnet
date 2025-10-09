#include "../include/client_handler.h"

#include <cstring>
#include <iostream>
#include <thread>
#include <unistd.h>

void handle_client(int client_socket, ClientManager &manager) {
  std::cout << "thread " << std::this_thread::get_id()
            << "cuidando do cliente socket: " << client_socket << std::endl;
  char buffer[1024];

  while (true) {
    memset(buffer, 0, 1024);
    int bytes_read = read(client_socket, buffer, 1024);

    if (bytes_read <= 0) {
      std::cout << "cliente " << client_socket << " desconectou." << std::endl;
      manager.remove(client_socket);
      break;
    }

    std::cout << "mensagem do cliente " << client_socket << ": " << buffer
              << std::endl;
  }

  close(client_socket);
}
