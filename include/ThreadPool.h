#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include <condition_variable>
#include <exception>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
 public:
  explicit ThreadPool(size_t num_threads = std::thread::hardware_concurrency())
      : stop(false) {
    for (size_t i = 0; i < num_threads; ++i) {
      workers.emplace_back([this] {
        while (true) {
          std::function<void()> task;

          std::unique_lock<std::mutex> lock(queue_mutex);
          cv.wait(lock, [this] { return stop || !tasks.empty(); });
          if (stop && tasks.empty()) {
            return;
          }
          task = std::move(tasks.front());
          tasks.pop();
          lock.unlock();

          try {
            task();
          } catch (const std::exception& e) {
            std::cerr << "Task exception: " << e.what() << std::endl;
          } catch (...) {
            std::cerr << "Unknown task exception." << std::endl;
          }
        }
      });
    }
  }

  template <class F, class... Args>
  auto enqueue(F&& task, Args&&... args)
      -> std::future<std::invoke_result_t<F, Args...>> {
    using return_type = std::invoke_result_t<F, Args...>;
    auto bound_task =
        std::bind(std::forward<F>(task), std::forward<Args>(args)...);
    auto promise = std::make_shared<std::promise<return_type>>();
    std::future<return_type> future = promise->get_future();

    std::unique_lock<std::mutex> lock(queue_mutex);
    tasks.emplace([bound_task, promise]() {
      try {
        if constexpr (std::is_void_v<return_type>) {
          bound_task();
          promise->set_value();
        } else {
          promise->set_value(bound_task());
        }
      } catch (...) {
        promise->set_exception(std::current_exception());
      }
    });
    lock.unlock();

    cv.notify_one();
    return future;
  }

  ~ThreadPool() {
    stop = true;
    cv.notify_all();
    for (std::thread& worker : workers) {
      if (worker.joinable()) {
        try {
          worker.join();
        } catch (const std::exception& e) {
          std::cerr << "Thread join exception: " << e.what() << std::endl;
        } catch (...) {
          std::cerr << "Unknown thread join exception" << std::endl;
        }
      }
    }
  }

  // Prevent copying and moving
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

 private:
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;
  std::mutex queue_mutex;
  std::condition_variable cv;
  bool stop;
};

#endif  // THREAD_POOL_H_
