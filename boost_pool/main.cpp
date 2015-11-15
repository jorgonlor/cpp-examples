#include <iostream>
#include <chrono>
#include <boost/pool/pool.hpp>
#include <boost/pool/object_pool.hpp>

struct A
{
    A() {}
    ~A() {}
    
    int data;
};

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
    
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "Pool time: " << elapsed_seconds.count() << std::endl;
    
    // Measure time allocationg with boost::pool
    // Object pool will call all A destructors when pool is destructed
    // (but it does not call constructors when allocating)
    start = std::chrono::system_clock::now();
    
    boost::object_pool<A> op;
    for(int i = 0; i < numIters; ++i)
        A* const pi = static_cast<A*>(op.malloc());
        
    end = std::chrono::system_clock::now();
    
    elapsed_seconds = end-start;
    std::cout << "Object pool time: " << elapsed_seconds.count() << std::endl;
    
    // Measure time allocationg using c++ new (leaked on purpose, only measuring allocation)
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
// Object pool: 0.17
// C++ new time: 0.22
