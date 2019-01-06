all: pool tbb_pool tbb_root

pool: pool.cpp threadsafe_queue.h
	clang++ -std=c++14 pool.cpp -o pool -pthread 

tbb_pool: tbb_pool.cpp
	clang++ -std=c++14 tbb_pool.cpp -o tbb_pool -pthread -ltbb

tbb_root: tbb_root.cpp
	clang++ -std=c++14 tbb_root.cpp -o tbb_root -pthread -ltbb

clean:
	rm -f pool tbb_pool tbb_root
