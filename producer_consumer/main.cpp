#include <iostream>
#include <functional>
#include <vector>
#include <queue>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

template <class T>
struct Task
{        
    std::function<void(T)> f;
    T data; 
};

template <class T>
class TaskQueue
{
public:

    TaskQueue(int ml) : maxLength(ml)
    {}    
       
    void push(const Task<T>& t)
    {
        std::unique_lock<std::mutex> lock(mtx);
                
        while(full())
        {
            cout << "Full queue, waiting..." << endl;
            producer_condition.wait(lock);
        }
                
        q.push(t);
        
        consumer_condition.notify_one();        
    }
    
    bool full() const
    {
        return q.size() == maxLength;
    }
    
    bool empty() const
    {
        return q.empty();
    }
    
    Task<T> pop()
    {
        std::unique_lock<std::mutex> lock(mtx);
        
        while(empty())
        {
            cout << "Empty queue, waiting..." << endl;
            consumer_condition.wait(lock);
        }
        
        Task<T> t = q.front();
        q.pop();
        
        producer_condition.notify_one();        
        
        return t;        
    }
    
private:
    
    std::queue< Task<T> > q;  
    unsigned int maxLength;
    
    std::condition_variable producer_condition;
    std::condition_variable consumer_condition;
    std::mutex mtx;
};

template <class T>
class ThreadPool
{
public:
    
    ThreadPool(int threadCount, int queueMaxLength) : task_queue(queueMaxLength) 
    { 
        for (int i=0; i < threadCount; ++i)
        {
            threads.push_back( std::thread( &ThreadPool::worker, this ) );
        }
    }
    
    void worker()
    {
        for(;;)
        {        
            Task<T> t = task_queue.pop();
            
            t.f(t.data);           
        }
    }
    
    void addTask(const Task<T>& t)
    {
        task_queue.push(t);
    }
    
private:    
    
    std::vector<std::thread> threads;
    TaskQueue<T> task_queue;
};

template <class T>
void example_task_routine(const T& data)
{
    cout << "I am task " << data << endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
    cout << "Task " << data << " finished" << endl;
}


int main(int argc, char **argv)
{    
    int data;
    
    ThreadPool<int> consumers_pool(3, 5);
    
    // Produce some data
    while(data != -1)
    {
        cin >> data;
                                
        Task<int> t;
        t.f = example_task_routine<int>;
        t.data = data;
        
        consumers_pool.addTask(t);
    }
    
	return 0;
}
