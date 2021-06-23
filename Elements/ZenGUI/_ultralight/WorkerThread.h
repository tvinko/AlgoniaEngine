#pragma once
#include "TaskQueue.h"
#include <thread>
#include <functional>
#include <memory>

template <class TaskType>
class WorkerThread {
public:
  WorkerThread() : is_running_(true) {
    thread_.reset(new std::thread(&WorkerThread::Run, this));
  }

  ~WorkerThread() {
    PostTask(std::bind(&WorkerThread::Stop, this));
    thread_->join();
  }

  void PostTask(TaskType task) {
    task_queue_.push(task);
  }

protected:
  void Run() {
    while (is_running_) {
      Task task = task_queue_.pop();
      task();
    }
  }

  void Stop() {
    is_running_ = false;
  }

  std::unique_ptr<std::thread> thread_;
  TaskQueue<TaskType> task_queue_;
  bool is_running_;
};