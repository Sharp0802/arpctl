#ifndef ARPCTL_EVENT_H
#define ARPCTL_EVENT_H

#include "framework.h"
#include "spinlock.h"


template<typename... Args>
class Event final
{
public:
	using Handler = std::function<void(Args...)>;

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

		_record.emplace_back(handler);
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
