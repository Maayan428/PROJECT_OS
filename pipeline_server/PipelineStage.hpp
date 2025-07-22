#pragma once

#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class PipelineStage {
public:
    using Task = std::function<void()>;

    PipelineStage();
    ~PipelineStage();

    void submit(Task task);

private:
    std::queue<Task> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    std::thread worker;
    std::atomic<bool> stop;

    void run(); 
};