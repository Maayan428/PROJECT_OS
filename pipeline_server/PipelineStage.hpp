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

    PipelineStage();          // Constructor: starts the worker thread
    ~PipelineStage();         // Destructor: stops the thread and cleans up
    void submit(Task task);   // Submit a new task to the pipeline

private:
    std::queue<Task> tasks;             // Queue to hold submitted tasks
    std::mutex mtx;                     // Mutex to protect access to the task queue
    std::condition_variable cv;         // Condition variable to notify the worker about new tasks
    std::thread worker;                 // The worker thread that runs submitted tasks
    std::atomic<bool> stop;             // Flag to indicate if the thread should stop

    void run();                         // Main loop that runs tasks from the queue
};