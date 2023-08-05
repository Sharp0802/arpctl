#ifndef ARPCTL_XTHREAD_H
#define ARPCTL_XTHREAD_H

#include <utility>

#include "framework.h"
#include "property.h"
#include "spinlock.h"

class Worker
{
public:
	enum class State : std::uint8_t
	{
		COMPLETED = 0,
		RESERVED = 1,
		OCCUPIED = 2,
		EXCEPTION = 3
	};

private:
	std::mutex _mutex;
	volatile State _state;
	volatile bool _token;
	std::function<void(const volatile bool*)> _fn;
	std::thread* _worker;

public:
	explicit Worker(std::function<void(const volatile bool*)> fn);
	~Worker();

public:
	void Start();

	void Stop();

	void Join();

	void Abort();

public:
	readonly<State> State{
			_get
			{
				return _state;
			}
	};

public:
	static size_t ID();
};


#endif //ARPCTL_XTHREAD_H
