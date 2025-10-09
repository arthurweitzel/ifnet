#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadDispatcher {
public:
  ThreadDispatcher(size_t num_threads);
  ~ThreadDispatcher();

  void enqueue(std::function<void()> task);

private:
  std::vector<std::thread> workers;
  // esse std function implica que uma task dentro de tasks é uma function
  std::queue<std::function<void()>> tasks;

  // abaixo servem para sincronizar as threads (acordar/dormir)
  std::mutex queue_mutex;
  std::condition_variable condition;
  bool stop;
};
