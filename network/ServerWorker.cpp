//
// Created by Cristobal Miranda, 2020
//

#include "ServerWorker.hpp"
#include "CacheServerTaskProcessor.hpp"
#include "ReplacementTaskProcessor.hpp"

template <class TaskProcessor> void ServerWorker<TaskProcessor>::start() {
  thread = std::make_unique<std::thread>(&self_t::main_loop, this);
}

template <class TaskProcessor> void ServerWorker<TaskProcessor>::stop() {
  {
    std::lock_guard<std::mutex> lg(m);
    running = true;
    stopped = true;
    cv.notify_all();
  }
  thread->join();
  thread = nullptr;
}

template <class TaskProcessor> void ServerWorker<TaskProcessor>::main_loop() {
  std::unique_lock<std::mutex> ul(m);
  for (;;) {
    cv.wait(ul, [this] { return running && task_processor.tasks_available(); });
    if (stopped) {
      break;
    }
    auto next_task = task_processor.get_server_task();
    if (!next_task) {
      continue;
    }

    next_task->process();
  }
}

template <class TaskProcessor>
ServerWorker<TaskProcessor>::ServerWorker(TaskProcessor &task_processor)
    : running(true), stopped(false), task_processor(task_processor) {
  start();
}

template <class TaskProcessor> void ServerWorker<TaskProcessor>::notify() {
  cv.notify_all();
}

template <class TaskProcessor> void ServerWorker<TaskProcessor>::pause() {
  std::lock_guard<std::mutex> lg(m);
  running = false;
}

template class ServerWorker<CacheServerTaskProcessor>;
template class ServerWorker<ReplacementTaskProcessor>;