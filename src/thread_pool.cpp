#include "../include/thread_pool.h"

ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                
                {
                    std::unique_lock<std::mutex> lock(this->queueMutex);
                    
                    // Wait for a task or stop signal
                    this->condition.wait(lock, [this] {
                        return this->stop || !this->tasks.empty();
                    });
                    
                    // Exit if stop signal and no more tasks
                    if (this->stop && this->tasks.empty()) {
                        return;
                    }
                    
                    // Get the next task
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                
                // Execute the task
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    
    // Wake up all threads
    condition.notify_all();
    
    // Join all threads
    for (std::thread& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

size_t ThreadPool::size() const {
    return workers.size();
}