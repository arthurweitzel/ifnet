#pragma once

#include <mutex>
#include <vector>

// isso deve mudar conforme a mensageria tomar forma, adicionando coisas como o
// nome e email do client
struct ClientInfo {
  int socket_fd;
};

class ClientManager {
public:
  void add(int client_socket);
  void remove(int client_socket);

private:
  std::vector<ClientInfo> clients;
  std::mutex mtx;
};
