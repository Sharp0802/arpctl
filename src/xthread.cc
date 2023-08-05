#include "pch.h"
#include "xthread.h"
#include "log.h"


size_t Worker::ID()
{
	static std::atomic<size_t> g{ 0 };
	static thread_local size_t r = g++;
	return r;
}

void Worker::Start()
{
	std::lock_guard lock(_mutex);

	if (_worker->joinable())
	{
		pthread_t pth = _worker->native_handle();
		pthread_cancel(pth);
	}

	_token = true;
	_worker = new std::thread([this](const volatile bool* token)
	{
		try
		{
			_state = State::OCCUPIED;
			_fn(token);
			_state = State::COMPLETED;
		}
		catch (const std::exception& e)
		{
			LOG(FAIL) << "===== EXCEPTION =====\n"
					  << e.what();
			_state = State::EXCEPTION;
		}
	}, &_token);
}

void Worker::Stop()
{
	_token = false;
}

void Worker::Join()
{
	if (_worker->joinable())
	{
		_worker->join();
	}
}

void Worker::Abort()
{
	std::lock_guard lock(_mutex);

	_token = false;
	if (_worker->joinable())
	{
		pthread_t pth = _worker->native_handle();
		pthread_cancel(pth);
	}
}

Worker::Worker(std::function<void(volatile const bool*)> fn) :
		_state(State::RESERVED),
		_token(false),
		_fn(std::move(fn)),
		_worker(nullptr)
{
}

Worker::~Worker()
{
	Abort();
}
