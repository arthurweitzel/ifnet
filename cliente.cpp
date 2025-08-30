#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
		
#define PORT 1337

int main() {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket == -1) {
        std::cerr << "Erro ao criar o socket" << std::endl;
        return 1;
    }

    // acha o endereço do server usando o port
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);


    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        std::cerr << "Endereço inválido ou não suportado" << std::endl;
        return 1;
    }

    // conecta ao server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Erro de conexão" << std::endl;
        return 1;
    }
	
    // manda uma mensagem
    const char* message = "Ola, servidor! Sou o cliente.";
    write(client_socket, message, strlen(message));
    std::cout << "Mensagem enviada para o servidor.\n";

    char buffer[1024] = {0};
    read(client_socket, buffer, 1024);
    std::cout << "Resposta do servidor: " << buffer << std::endl;

    close(client_socket);
}