#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>

namespace Engine
{
    // Courtesy of https://stackoverflow.com/a/32593825
    class ThreadPool {
    private:
        bool should_terminate = false;           // Tells threads to stop looking for jobs
        std::mutex queue_mutex;                  // Prevents data races to the job queue
        std::condition_variable mutex_condition; // Allows threads to wait on new jobs or termination 
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> jobs;

    public:
        void Start()
    	{
            const uint32_t num_threads = std::thread::hardware_concurrency(); // Max # of threads the system supports
            for (uint32_t ii = 0; ii < num_threads; ++ii) {
                threads.emplace_back(&ThreadPool::ThreadLoop, this);
            }
        }

        void QueueJob(const std::function<void()>& job)
    	{
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                jobs.push(job);
            }
            mutex_condition.notify_one();
        }

        void Stop()
    	{
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                should_terminate = true;
            }
            mutex_condition.notify_all();
            for (std::thread& active_thread : threads) {
                active_thread.join();
            }
            threads.clear();
        }

        bool Busy()
        {
            bool poolbusy;
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                poolbusy = !jobs.empty();
            }
            return poolbusy;
        }

    private:
        void ThreadLoop()
    	{
            while (true) 
            {
                std::function<void()> job;
                {
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    mutex_condition.wait(lock, [this] {
                        return !jobs.empty() || should_terminate;
                    });
                    if (should_terminate) {
                        return;
                    }
                    job = jobs.front();
                    jobs.pop();
                }
                job();
            }
        }
    };
}