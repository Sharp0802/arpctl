#ifndef ARPCTL_PROPERTY_H
#define ARPCTL_PROPERTY_H

#include "framework.h"

template<typename T>
class __property final
{
private:
	const std::function<T()> _getter;
	const std::function<void(T&)> _setter;

public:
	explicit __property(std::function<T()> getter, std::function<void(T&)> setter) :
			_getter(getter),
			_setter(setter)
	{
	}

public:
	const __property<T>& operator=(T rhs) const // NOLINT(*-unconventional-assign-operator)
	{
		_setter(rhs);
		return *this;
	}

	T Get() const
	{
		return _getter();
	}
};

template<typename T>
class __readonly final
{
private:
	const std::function<T()> _getter;

public:
	explicit __readonly(std::function<T()> getter) : _getter(getter)
	{
	}

public:
	T Get() const
	{
		return _getter();
	}
};

template<typename T>
using property = __property<T>;

template<typename T>
using readonly = const __readonly<T>;

#define _get [this]() -> auto
#define _set [this](auto& value) -> void


#define _static_get []() -> auto
#define _static_set [](auto& value) -> void

#endif //ARPCTL_PROPERTY_H
