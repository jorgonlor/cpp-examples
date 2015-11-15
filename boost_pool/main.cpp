#include <iostream>
#include <chrono>
#include <boost/pool/pool.hpp>

int main(int argc, char **argv)
{
    const int numIters = 8000000;
    
    // Measure time allocationg with boost::pool
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    
    boost::pool<> p(sizeof(int));
    for(int i = 0; i < numIters; ++i)
        int* const pi = static_cast<int*>(p.malloc());
        
    end = std::chrono::system_clock::now();
    
    // Measure time allocationg using c++ new (leaked on purpose, only measuring allocation)
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "Pool time: " << elapsed_seconds.count() << std::endl;
    
    start = std::chrono::system_clock::now();
    
    for(int i = 0; i < numIters; ++i)
        int* pi = new int;
        
    end = std::chrono::system_clock::now();
    
    elapsed_seconds = end-start;
    std::cout << "C++ new time: " << elapsed_seconds.count() << std::endl;
    
	return 0;
}  // Pool ints are automatically deallocated here when destructing the pool

// Obtained output:
// Pool time: 0.14
// C++ new time: 0.22
