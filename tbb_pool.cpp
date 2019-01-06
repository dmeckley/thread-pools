#include <iostream>
#include <random>
#include <thread>

#include <tbb/tbb.h>



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


struct WorkFunctor
{
    void operator()() const
    {
        work();
    }
};


int main()
{
    tbb::task_group tg;

    // We can't directly give the function to the thread pool, like this:
    //
    // tg.run(work);
    //
    // Because the run() method expects a functor.

    // So we can use a quick functor class:
    tg.run(WorkFunctor{});

    // Or we can make functors with lambdas:
    for (int i = 0; i < 5; i++)
        tg.run( [](){work();} );

    auto done = [](){ std::cout << "The work is done" << std::endl; };
    tg.run(done);
    
    // Wait for all the tasks to complete.
    tg.wait();

    return 0;
}