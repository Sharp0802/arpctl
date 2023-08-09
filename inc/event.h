#ifndef ARPCTL_EVENT_H
#define ARPCTL_EVENT_H

#include "framework.h"
#include "pstl/spinlock.h"


template<typename... Args>
class EventHandler final
{
public:
	const size_t ID;

private:
	inline static std::atomic<size_t> _gID;
	std::function<void(Args...)> _fn;

public:
	explicit EventHandler(std::function<void(Args...)> fn) : ID(_gID++), _fn(fn)
	{
	}

public:
	void operator()(Args... args) const
	{
		_fn(args...);
	}

	bool operator==(const EventHandler& rhs) const
	{
		return ID == rhs.ID;
	}
};

namespace std
{
	template<typename... Args>
	class hash<EventHandler<Args...>>
	{
	public:
		size_t operator()(const EventHandler<Args...>& arg) const noexcept
		{
			return arg.ID;
		}
	};
}


template<typename... Args>
class Event final
{
public:
	using Handler = EventHandler<Args...>;

private:
	pstl::spinlock _spin;
	std::unordered_set<Handler> _record;

public:
	void operator()(Args... args) const
	{
		for (const auto& handler: _record)
			handler(args...);
	}

	void operator+=(Handler handler)
	{
		std::lock_guard lock(_spin);

		_record.emplace(handler);
	}

	void operator-=(Handler handler)
	{
		std::lock_guard lock(_spin);

		auto p = _record.find(handler);
		if (p != _record.end())
			_record.erase(p);
	}
};


#endif //ARPCTL_EVENT_H
