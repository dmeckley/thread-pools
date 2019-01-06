#include <iostream>
#include <cmath>
#include <vector>
#include <random>

#include <tbb/tbb.h>


int newton_sqrt(int n)
{
    int x = std::pow(2, 16);

    while(true)
    {
        int y = (x + n/x) / 2;
        if (y >= x)
            return x;
        x = y;
    }
}

const int CHUNK_SIZE = 1000;

int main()
{
    
    std::vector<int> numbers(10'000'000);

    std::mt19937 gen(0);
    std::uniform_int_distribution<int> rand_num(1, 100'000);
    
    for (int& num: numbers)
        num = rand_num(gen);

    tbb::task_group tg;

    for (int start = 0; start < numbers.size(); start += CHUNK_SIZE)
    {
        tg.run([start, &numbers]()
        {
            for (int i = start; i < start + CHUNK_SIZE; i++)
                numbers[i] = newton_sqrt(numbers[i]);
        });
    }
    tg.wait();

    return 0;
}