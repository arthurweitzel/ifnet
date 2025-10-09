#include "../include/client_manager.h"
#include "../include/server.h"
#include "../include/thread_dispatcher.h"
#include <iostream>

#define PORT 1337

int main() {
  try {
    // usando o numero de nucleos que temos como numero de threads
    unsigned int num_threads = std::thread::hardware_concurrency();

    std::cout << "iniciando o dispatcher com " << num_threads << " threads"
              << std::endl;

    ThreadDispatcher dispatcher(num_threads);
    ClientManager client_manager;
    Server server(PORT, dispatcher, client_manager);

    server.start();

  } catch (const std::exception &e) {
    std::cerr << "erro " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
