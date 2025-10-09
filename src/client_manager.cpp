#include "../include/client_manager.h"
#include <algorithm>

void ClientManager::add(int client_socket) {
  std::lock_guard<std::mutex> lock(mtx);
  clients.push_back({client_socket});
}

void ClientManager::remove(int client_socket) {
  std::lock_guard<std::mutex> lock(mtx);

  // padrao erase-remove, o chatgpt sugeriu essa bomba ai apra remover todas as
  // ocorrencias de um client
  clients.erase(std::remove_if(clients.begin(), clients.end(),
                               [client_socket](const ClientInfo &client) {
                                 return client.socket_fd == client_socket;
                               }),
                clients.end());
}
