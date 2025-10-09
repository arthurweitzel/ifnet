#include "../include/thread_dispatcher.h"
#include <stdexcept>

ThreadDispatcher::ThreadDispatcher(size_t num_threads) : stop(false) {
  for (size_t i = 0; i < num_threads; ++i) {
    // simplesmente constroi uma thread diretamente dentro do vector, sem ter
    // que criar uma thread e depois mover para dentro
    workers.emplace_back([this] {
      while (true) {
        std::function<void()> task;

        std::unique_lock<std::mutex> lock(this->queue_mutex);

        // essa é a parte mais importante, o wait libera o lock, poe a thread
        // pra dormir e aguarda um notify de outra thread

        // se acrodada, ela ativa o lock e le o lambda
        //  condition.wait (lock, predicado [nesse caso um lambda])
        this->condition.wait(
            lock, [this] { return this->stop || !this->tasks.empty(); });

        if (this->stop && this->tasks.empty()) {
          return;
        }

        task = std::move(this->tasks.front());
        this->tasks.pop();

        task();
      }
    });
  }
}

ThreadDispatcher::~ThreadDispatcher() {
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    stop = true;
  }
  condition.notify_all(); // acorda todas as threads para que terminem
  for (std::thread &worker : workers) {
    // esse join aguarda com que todas as threads encerrem para que o main()
    // termine
    worker.join();
  }
}

void ThreadDispatcher::enqueue(std::function<void()> task) {
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    if (stop) {
      throw std::runtime_error("enqueue on stopped the dispatcher");
    }
    tasks.emplace(std::move(task));
  }
  condition.notify_one(); // acorda uma thread para executar a nova tarefa
}
