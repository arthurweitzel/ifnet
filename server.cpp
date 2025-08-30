#include <iostream>
#include <netinet/in.h> // sockaddr_in
#include <sys/socket.h> // socket, bind, listen, accept
#include <unistd.h> // read, write, close
#include <chrono> // usar o sleep pra testar as threads
#include <cstring> 
#include <thread>

#define PORT 1337
#define MAX_CONNECTIONS 10

void handle_client(int client_socket) {
	char buffer [1024] = {0};
	int bytes_read = read(client_socket, buffer, 1024);
	std::this_thread::sleep_for(std::chrono::seconds(3));	

	if (bytes_read > 0) {
		std::cout << "message received from " << client_socket << ": ", buffer;

		const char* response = "received by server\n";
		write(client_socket, response, strlen(response));
	} else {
		std::cout << "couldnt read from client\n";
	}

	close(client_socket);
	std::cout << "connection closed\n";
}

int main() {
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // aceita qualquer ip para conexao
    server_addr.sin_port = htons(PORT);       // converte a porta para network byte order

	bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	listen(server_fd, MAX_CONNECTIONS);

	while (true) {
		sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);
			
		int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
		std::thread client_thread(handle_client, client_socket);

		// detach() permite que a thread rode de forma independente da thread principal
        client_thread.detach(); 
	}
	close(server_fd);
}