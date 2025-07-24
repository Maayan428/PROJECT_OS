#include "PipelineStage.hpp"

/**
 * @brief Constructor that initializes the pipeline stage.
 *        It starts a worker thread that continuously runs tasks from a queue.
 */
PipelineStage::PipelineStage() : stop(false) {
    worker = std::thread([this]() { run(); });
}

/**
 * @brief Destructor that stops the worker thread gracefully.
 *        It sets the stop flag and waits for the thread to join.
 */
PipelineStage::~PipelineStage() {
    {
        std::unique_lock<std::mutex> lock(mtx);
        stop = true;
        cv.notify_all(); // Wake up the worker thread if it's waiting
    }
    if (worker.joinable()) {
        worker.join(); // Wait for the worker thread to finish
    }
}

/**
 * @brief Submits a new task to the task queue.
 *        Notifies the worker thread that a new task is available.
 * 
 * @param task The task (callable) to be added to the queue.
 */
void PipelineStage::submit(Task task) {
    std::unique_lock<std::mutex> lock(mtx);
    tasks.push(std::move(task));
    cv.notify_one(); // Notify one waiting thread that a new task is available
}

/**
 * @brief Continuously runs in the worker thread.
 *        Waits for new tasks to be submitted and executes them.
 */
void PipelineStage::run() {
    while (true) {
        Task task;
        {
            std::unique_lock<std::mutex> lock(mtx);
            // Wait until stop is true or a new task is available
            cv.wait(lock, [&]() { return stop || !tasks.empty(); });
            if (stop && tasks.empty()) return; // Exit if stopping and no more tasks
            task = std::move(tasks.front()); // Take the task from the front of the queue
            tasks.pop(); // Remove the task from the queue
        }
        task(); // Execute the task
    }
}