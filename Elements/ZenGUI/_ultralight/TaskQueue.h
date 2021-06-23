#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

typedef std::function<void()> Task;
// A thread-safe task queue.
template <class T>
class TaskQueue
{
public:
  TaskQueue(void)
    : queue_()
    , queue_lock_()
    , wait_for_task_()
  {}

  ~TaskQueue(void)
  {}

  // Push element to back of queue.
  void push(T t)
  {
    std::lock_guard<std::mutex> lock(queue_lock_);
    queue_.push(t);
    wait_for_task_.notify_one();
  }

  // Pop element from front.
  // Blocks until a task is available.
  T pop()
  {
    std::unique_lock<std::mutex> lock(queue_lock_);
    while (queue_.empty())
      wait_for_task_.wait(lock);

    T val = queue_.front();
    queue_.pop();
    return val;
  }

  bool empty()
  {
    std::unique_lock<std::mutex> lock(queue_lock_);
    return queue_.empty();
  }

private:
  std::queue<T> queue_;
  mutable std::mutex queue_lock_;
  std::condition_variable wait_for_task_;
};