// ThreadPool.cpp
#include "ThreadPool.h"
#include "Logger.h"

ThreadPool::ThreadPool(size_t num_threads) {
    Logger::log("ThreadPool: starting " + std::to_string(num_threads) + " workers");
    for (size_t i = 0; i < num_threads; ++i) {
        workers_.emplace_back([this, i]() { worker_loop(static_cast<int>(i)); });
    }
}

ThreadPool::~ThreadPool() {
    stop_ = true;
    cv_.notify_all();
    for (auto& w : workers_) {
        if (w.joinable()) w.join();
    }
    Logger::log("ThreadPool: all workers stopped");
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        tasks_.push(std::move(task));
    }

    cv_.notify_one();
}

void ThreadPool::worker_loop(int worker_id) {
    Logger::log("  worker " + std::to_string(worker_id) + " ready");

    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(mutex_);

            cv_.wait(lock, [this]() {
                return !tasks_.empty() || stop_;
            });

            if (stop_ && tasks_.empty()) {
                Logger::log("  worker " + std::to_string(worker_id) + " exiting");
                return;
            }

            task = std::move(tasks_.front());
            tasks_.pop();

            Logger::log("  worker " + std::to_string(worker_id)
                        + " picked up a task (queue size: "
                        + std::to_string(tasks_.size()) + ")");
        }
        task();
    }
}