#ifndef ARPCTL_JOBSYS_H
#define ARPCTL_JOBSYS_H

#include "pstl/channeling_queue.h"
#include "xthread.h"


using Job = std::function<void()>;

struct ThreadContext final
{
	pstl::channeling_queue<Job> LocalQueue;
	std::thread Thread;
};

class JobSystem final
{
private:
	inline static JobSystem* _instance;

private:
	bool _init;
	volatile bool _token;
	pstl::spinlock _sync;
	const std::chrono::milliseconds _timeout;
	std::vector<ThreadContext> _threads{ std::thread::hardware_concurrency() };

public:
	JobSystem(std::chrono::milliseconds timeout);

	~JobSystem();

	JobSystem& GetInstance();

private:
	void InitializeThread(ThreadContext* ctx);

	void Initialize();

	void Finalize();

public:
	bool Start();

	void Stop();

public:
	void operator +=(Job job);
};


#endif //ARPCTL_JOBSYS_H
