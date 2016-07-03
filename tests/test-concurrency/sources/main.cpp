#include <Windows.h>

#include <pc/ProgressCounter.h>

#include <mutex>
#include <thread>
#include <list>
#include <chrono>
#include <random>


using Task = std::function<void ()>;

class TaskQueue final
{
public:

	void PushTask( Task&& task )
	{
		m_tasks.emplace_back( task );
	};
	
	Task PullTask()
	{
		std::unique_lock<std::mutex> lock( m_latch );
		if( m_tasks.empty() )
		{
			return {};
		};

		auto task = m_tasks.front();
		m_tasks.pop_front();
		return task;
	};

	inline void Lock() const				{ m_latch.lock(); };
	inline void Unlock() const				{ m_latch.unlock(); };

	inline const size_t GetCount() const	{ std::unique_lock<std::mutex> lock( m_latch ); return m_tasks.size(); };

	inline const bool IsEmpty() const		{ std::unique_lock<std::mutex> lock( m_latch ); return m_tasks.empty(); };

private:
	mutable std::mutex	m_latch;
	std::list<Task>		m_tasks;
};

void ThreadFunction( TaskQueue& tasks )
{
	while( !tasks.IsEmpty() )
	{
		{
			auto task = tasks.PullTask();
			// Process this task.
			task();
		};
	};
};

int main( int argc, char* argv[] )
{
	// Our progress counter.
	auto counter = pc::ProgressCounter::Create();
	// Our tasks queue.
	TaskQueue tasks;

	// Produce the tasks.
	std::mt19937 rnd_device( (uint32_t)time( 0 ) );
	for( int32_t task_id = 0; task_id < 680; ++task_id )
	{
		auto point = counter->ProduceGoal();
		std::chrono::milliseconds interval{ 600 + rnd_device() % 1400 };
		tasks.PushTask( { [point, interval](){ std::this_thread::sleep_for( interval ); } } );
	};

	// Produce the workers.
	tasks.Lock();
	size_t threads_count = std::thread::hardware_concurrency();
	while( threads_count-- > 0 )
	{
		std::thread t( ThreadFunction, std::ref( tasks ) );
		t.detach();
	};
	tasks.Unlock();

	// Display the progress of tasks completion until the progress reaches 100%
	do
	{
		printf( "\rCurrent progress : %3.3f%%; Tasks remained : %zd;", 100.0f * counter->GetFinisedPercent(), tasks.GetCount() );
		std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
	}
	while( counter->GetRemainsPercent() > 0.0f );
};
