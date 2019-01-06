#include <mutex>
#include <queue>
#include <atomic>
#include <functional>

#include <thread>
#include <iostream>
#include <random>

#include "threadsafe_queue.h"


class join_threads
{
public:
    explicit join_threads(std::vector<std::thread>& threads_):
        threads(threads_)
    {}
 
    ~join_threads()
    {
        for (auto& thread : threads)
            if (thread.joinable())
                thread.join();
    }

private:
    std::vector<std::thread>& threads;
};


class thread_pool
{
    std::atomic_bool done;
    threadsafe_queue<std::function<void()>> work_queue;
    std::vector<std::thread> threads;
    join_threads joiner;

    void worker_thread()
    {
        // This is a busy-wait loop!
        while (!done)
        {
            std::function<void()> task;
            if (work_queue.try_pop(task))
            {
                task();
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }

public:
    thread_pool():
        done(false), joiner(threads)
    {
        unsigned const thread_count = std::thread::hardware_concurrency();

        try
        {
            for (unsigned i = 0; i < thread_count; i++)
                threads.push_back(
                    std::thread(&thread_pool::worker_thread, this));
        }
        catch (...)
        {
            done = true;
            throw;
        }

        std::cout << "thread_pool created with " << thread_count
                  << " threads." << std::endl;
    }

    ~thread_pool()
    {
        done = true;
    }

    template<typename FunctionType>
    void submit(FunctionType f)
    {
        work_queue.push(std::function<void()>(f));
    }
};


void work()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> id(1, 100);
    std::uniform_int_distribution<int> delay(1800, 2200);
    int work_id = id(gen);
    int delay_time = delay(gen);

    std::cout << "Begin work " << work_id << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_time));
    std::cout << "Finish work " << work_id << std::endl;
}


int main()
{
    auto done = [](){ std::cout << "The work is done" << std::endl; };
    
    for (int i = 0; i < 6; i++)
        work();
    done();

    thread_pool pool;
    for (int i = 0; i < 6; i++)
        pool.submit(work);
    pool.submit(done);

    // Sleep for a while, because there's no way to wait for the pool threads
    // to complete their tasks.
    std::this_thread::sleep_for(std::chrono::seconds(8));

    return 0;
}