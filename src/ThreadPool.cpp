#include "Threadpool.h"

ThreadPool::ThreadPool(size_t threadCount) {
    for (size_t i = 0; i < threadCount; ++i){
        workers.emplace_back([this]{
            while (true){
                std::function<void()> task;

                {
                    std::unique_lock lock(mutex);
                    cv.wait(lock, [this] { return stop || !tasks.empty(); });

                    if(stop && tasks.empty())
                        return;

                    task = std::move(tasks.front());
                    tasks.pop();
                }
                task();
            }
        });
    }
}

void ThreadPool::enqueue(std::function<void()> task){
    {
        std::lock_guard lock(mutex);
        tasks.push(std::move(task));
    }
    cv.notify_one();
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard lock(mutex);
        stop = true;
    }
    cv.notify_all();
    for (auto& t : workers)
        t.join();
}