//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_SERVERWORKER_HPP
#define RDFCACHEK2_SERVERWORKER_HPP

#include <condition_variable>
#include <mutex>
#include <thread>

template <class TaskProcessor> class ServerWorker {
  std::condition_variable cv;
  std::mutex m;

  bool running;
  bool stopped;
  TaskProcessor &task_processor;
  std::unique_ptr<std::thread> thread;

  using self_t = ServerWorker<TaskProcessor>;

public:
  ServerWorker(TaskProcessor &task_processor);
  void start();
  void stop();

private:
  void main_loop();
};

#endif // RDFCACHEK2_SERVERWORKER_HPP
