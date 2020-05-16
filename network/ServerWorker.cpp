//
// Created by Cristobal Miranda, 2020
//

#include "ServerWorker.hpp"
#include "CacheServerTaskProcessor.hpp"

template<class TaskProcessor>
void ServerWorker<TaskProcessor>::start() {
  thread = std::make_unique<std::thread>(&self_t::main_loop, this);
}

template<class TaskProcessor>
void ServerWorker<TaskProcessor>::stop() {
  {
    std::lock_guard<std::mutex> lg(m);
    running = true;
    stopped = true;
    cv.notify_all();
  }
  thread->join();
  thread = nullptr;
}

template<class TaskProcessor>
void ServerWorker<TaskProcessor>::main_loop() {
  std::unique_lock<std::mutex> ul(m);
  for(;;){
    cv.wait(ul, [this]{return running;});
    if(stopped){
      break;
    }
    auto next_task = task_processor.get_server_task();
    if(!next_task){
      continue;
    }

    next_task->process();
  }
}

template<class TaskProcessor>
ServerWorker<TaskProcessor>::ServerWorker(TaskProcessor &task_processor) :
running(false), stopped(false), task_processor(task_processor) {
  start();
}


template class
ServerWorker<CacheServerTaskProcessor>;