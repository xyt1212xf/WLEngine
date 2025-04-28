#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

namespace WL
{
	class CThreadPool 
	{
	public:
		static void setThreadName(const std::string& szName, UINT32 unThreadID )
		{
			const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
			typedef struct tagTHREADNAME_INFO
			{
				DWORD dwType; // Must be 0x1000.
				LPCSTR szName; // Pointer to name (in user addr space).
				DWORD dwThreadID; // Thread ID (-1=caller thread).
				DWORD dwFlags; // Reserved for future use, must be zero.
			} THREADNAME_INFO;
#pragma pack(pop)

			THREADNAME_INFO info;
			info.dwType = 0x1000;
			info.szName = szName.c_str();
			info.dwThreadID = unThreadID;
			info.dwFlags = 0;

			__try
			{
				RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
			}
		}
		CThreadPool(int threads = -1);
		~CThreadPool();
		template<class F, class... Args>
		auto enqueueJoin(F&& f, Args&&... args);// ->std::future<typename std::result_of<F(Args...)>::type>;

		template<class F, class... Args>
		void enqueueDetach(F&& f, Args&&... args);// ->std::future<typename std::result_of<F(Args...)>::type>;
	private:
		// need to keep track of threads so we can join them
		std::vector<std::thread> mWorkers;
		// the task queue
		std::queue<std::function<void()> > mTasks;

		// synchronization
		std::mutex mQueueMutex;
		std::condition_variable mCondition;
		bool mStop = false;
	};

	// the constructor just launches some amount of workers
	INLINE CThreadPool::CThreadPool(int nMaxthreads)
	{
		int nthreads = (-1 == nMaxthreads ? std::thread::hardware_concurrency() : nMaxthreads);
		for (int i = 0; i < nthreads; ++i)
		{
			mWorkers.emplace_back(
				[this]()
				{
					for (;;)
					{
						std::function<void()> task;
						{
							std::unique_lock<std::mutex> lock(this->mQueueMutex);
							this->mCondition.wait(lock,
								[this]()->bool
								{ 
									return this->mStop || !this->mTasks.empty(); 
								});
							if (this->mStop && this->mTasks.empty())
							{
								return;
							}
							task = std::move(this->mTasks.front());
							this->mTasks.pop();
						}
						task();
					}
				}
				);
		}
	}

	template<class F, class... Args>
	auto CThreadPool::enqueueJoin(F&& f, Args&&... args)//->std::future<typename std::result_of<F(Args...)>::type>
	{
		using return_type = typename std::result_of<F(Args...)>::type;

		auto task = std::make_shared< std::packaged_task<return_type()> >(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
			);

		std::future<return_type> res = task->get_future();
		{
			std::unique_lock<std::mutex> lock(mQueueMutex);

			// don't allow enqueueing after stopping the pool
			if (mStop)
			{
				throw std::runtime_error("enqueue on stopped ThreadPool");
			}
			mTasks.emplace([task]() { (*task)(); });
		}
		mCondition.notify_one();
		return res;
	}

	template<class F, class... Args>
	void CThreadPool::enqueueDetach(F&& f, Args&&... args)
	{
		{
			std::unique_lock<std::mutex> lock(mQueueMutex);

			// don't allow enqueueing after stopping the pool
			if (mStop)
			{
				throw std::runtime_error("enqueue on stopped ThreadPool");
			}
			mTasks.emplace([&]() { f(std::forward<Args>(args)...); });
		}
		mCondition.notify_one();
	}
	// the destructor joins all threads
	inline CThreadPool::~CThreadPool()
	{
		{
			std::unique_lock<std::mutex> lock(mQueueMutex);
			mStop = true;
		}
		mCondition.notify_all();
		for (std::thread& worker : mWorkers)
		{
			worker.join();
		}
	}
}
