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

	if (_worker && _worker->joinable())
	{
		pthread_t pth = _worker->native_handle();
		pthread_cancel(pth);
	}

	_token = true;
	_state = State::OCCUPIED;
	_worker = new std::thread([this](const volatile bool* token)
	{
		try
		{
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
	if (_worker && _worker->joinable())
	{
		_worker->join();
	}
}

std::future_status Worker::Join(std::chrono::milliseconds timeout)
{
	std::lock_guard lock(_mutex);

	if (!_worker || !_worker->joinable())
		return std::future_status::ready;

	auto wait = std::async(std::launch::async, [this]() { _worker->join(); });
	return wait.wait_for(timeout);
}

void Worker::Abort()
{
	std::lock_guard lock(_mutex);

	_token = false;
	if (_worker && _worker->joinable())
	{
		pthread_t pth = _worker->native_handle();
		pthread_cancel(pth);
	}
	_state = State::COMPLETED;
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
