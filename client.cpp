#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 1337
const char *SERVER_IP = "127.0.0.1";

int main() {
  int client_socket = socket(AF_INET, SOCK_STREAM, 0);

  if (client_socket < 0) {
    std::cerr << "nao foi possivel criar o socket" << std::endl;
    return 1;
  }

  sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT); // garante a ordem dos bytes enviados

  // inet_pton = "internet presentation to network", só converte a string do ip
  // para o padrao binario
  if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
    std::cerr << "IP invalido" << std::endl;
    close(client_socket);
    return 1;
  }

  // 3 way hs do hewerton
  if (connect(client_socket, (struct sockaddr *)&server_addr,
              sizeof(server_addr)) < 0) {
    std::cerr << "falha na conexão" << std::endl;
    close(client_socket);
    return 1;
  }

  std::cout << "conectado, 'quit' para encerrar" << std::endl;

  std::string message;
  while (true) {
    std::cout << "mensagem > ";
    std::getline(std::cin, message); // usa getline para ler a linha inteira

    if (message.empty()) {
      continue;
    }

    if (message == "quit") {
      break;
    }

    // write() é uma syscall para escrever dados em um
    // descritor de arquivo
    ssize_t bytes_sent =
        write(client_socket, message.c_str(), message.length());
    if (bytes_sent < 0) {
      std::cerr << "" << std::endl;
      break;
    }
  }

  std::cout << "quiting.." << std::endl;
  close(client_socket);

  return 0;
}
