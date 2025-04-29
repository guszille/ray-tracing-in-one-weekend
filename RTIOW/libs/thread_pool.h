#pragma once

#include <vector>
#include <mutex>
#include <queue>
#include <thread>
#include <functional>
#include <condition_variable>

class thread_pool
{
public:
	thread_pool() : stop(false), num_enqueued_tasks(0), num_dispatched_tasks(0) { initalize(std::thread::hardware_concurrency()); }
	thread_pool(int num_threads) : stop(false), num_enqueued_tasks(0), num_dispatched_tasks(0) { initalize(num_threads); }

	template<class F>
	void enqueue(F&& task)
	{
		std::unique_lock<std::mutex> lock(queue_mutex);

		tasks.emplace(std::forward<F>(task));
		num_enqueued_tasks++;

		lock.unlock();
		condition.notify_one();
	}

	void terminate()
	{
		std::unique_lock<std::mutex> lock(queue_mutex);

		stop = true;

		lock.unlock();
		condition.notify_all();

		for (std::thread& worker : workers)
		{
			worker.join();
		}
	}

private:
	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks;
	std::mutex queue_mutex;
	std::condition_variable condition;
	bool stop;
	uint32_t num_enqueued_tasks, num_dispatched_tasks;

	void initalize(int num_threads)
	{
		for (int n = 0; n < num_threads; ++n)
		{
			workers.emplace_back([this] {
				while (true)
				{
					std::unique_lock<std::mutex> lock(queue_mutex);

					condition.wait(lock, [this] { return stop || !tasks.empty(); });
					if (stop && tasks.empty()) { return; }

					// Retrieve a task from the queue.
					auto task = std::move(tasks.front());
					tasks.pop();
					num_dispatched_tasks++;

					std::clog << '\r' << "Tasks: " << num_dispatched_tasks << "/" << num_enqueued_tasks << "        " << std::flush;

					lock.unlock();

					// Execute the task.
					task();
				}
			});
		}
	}
};
