#include "PipelineStage.hpp"

PipelineStage::PipelineStage() : stop(false) {
    worker = std::thread([this]() { run(); });
}

PipelineStage::~PipelineStage() {
    {
        std::unique_lock<std::mutex> lock(mtx);
        stop = true;
        cv.notify_all();
    }
    if (worker.joinable()) {
        worker.join();
    }
}

void PipelineStage::submit(Task task) {
    std::unique_lock<std::mutex> lock(mtx);
    tasks.push(std::move(task));
    cv.notify_one();
}

void PipelineStage::run() {
    while (true) {
        Task task;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&]() { return stop || !tasks.empty(); });
            if (stop && tasks.empty()) return;
            task = std::move(tasks.front());
            tasks.pop();
        }
        task();
    }
}