#include "pch.h"
#include "jobsys.h"

JobSystem::JobSystem(std::chrono::milliseconds timeout) : _timeout(timeout)
{
	_instance = this;
}

JobSystem::~JobSystem()
{
	Stop();
}

JobSystem& JobSystem::GetInstance()
{
	return *_instance;
}

void JobSystem::InitializeThread(ThreadContext* ctx)
{
	ctx->Thread = std::thread([this](ThreadContext* ctx) {
		while (_token)
		{
			auto fn = ctx->LocalQueue.pop();
			if (!fn)
				sched_yield();
			fn.value()();
		}
	}, ctx);
}

void JobSystem::Initialize()
{
	_token = true;
	for (auto& cur: _threads)
		InitializeThread(&cur);
	_init = true;
}

void JobSystem::Finalize()
{
	_token = false;
	std::vector<std::future<void>> when(_threads.size());
	for (auto& cur: _threads)
	{
		when.emplace_back(std::async(std::launch::async, [this, &cur]()
		{
			auto wait = std::async(std::launch::async, [&cur]() {
				cur.Thread.join();
			});
			auto status = wait.wait_for(_timeout);
			if (status == std::future_status::timeout)
			{
				pthread_t pth = cur.Thread.native_handle();
				pthread_cancel(pth);
			}
		}));
	}
	for (auto& wait: when)
	{
		wait.wait();
	}
	_init = false;
}

bool JobSystem::Start()
{
	std::lock_guard lock(_sync);
	if (_init) return false;

	Initialize();

	return true;
}

void JobSystem::Stop()
{
	std::lock_guard lock(_sync);
	if (!_init) return;

	Finalize();
}

void JobSystem::operator+=(Job job)
{
	ThreadContext* ctx = nullptr;
	size_t min;
	for (auto& cur: _threads)
	{
		/*
		 * do NOT lock for local-queue
		 * integrity not required for this scenario
		 * */
		if (min <= cur.LocalQueue.size())
			continue;
		min = cur.LocalQueue.size();
		ctx = &cur;
	}
	ctx->LocalQueue.push(job);
}
