#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>

class ThreadPool {
public:
    // Constructor creates the thread pool with the specified number of threads
    ThreadPool(size_t numThreads);
    
    // Destructor stops all threads and cleans up
    ~ThreadPool();
    
    // Add a task to the thread pool and get a future for the result
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::invoke_result<F, Args...>::type>;
    
    // Get the number of threads in the pool
    size_t size() const;
    
private:
    // Worker threads
    std::vector<std::thread> workers;
    
    // Task queue
    std::queue<std::function<void()>> tasks;
    
    // Synchronization
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
};

// Implementation of the enqueue method
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) 
    -> std::future<typename std::invoke_result<F, Args...>::type> {
    
    using return_type = typename std::invoke_result<F, Args...>::type;
    
    // Create a shared pointer to the packaged task
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    
    // Get the future result before we move the task
    std::future<return_type> result = task->get_future();
    
    // Lock the queue and add the task
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        
        // Don't allow enqueueing after stopping the pool
        if (stop) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        
        // Add the task to the queue
        tasks.emplace([task]() { (*task)(); });
    }
    
    // Notify one waiting thread that there's a new task
    condition.notify_one();
    
    return result;
}

#endif // THREAD_POOL_H