#ifndef ARPCTL_MAC_H
#define ARPCTL_MAC_H

#include "framework.h"
#include "convention.h"
#include "property.h"


decl_DTO(MAC)
{
	uint8_t seg[6];
};

class MAC;

namespace std
{
	template<>
	struct less<MAC>
	{
		constexpr bool operator()(const MAC& x, const MAC& y) const;
	};
}

class MAC final
{
	friend struct std::less<MAC>;

private:
	inline static thread_local std::unordered_map<std::string_view, DTO(MAC)> _self;

private:
	DTO(MAC) _dto;

public:
	MAC();

	MAC(const MAC& rhs);

	explicit MAC(void* raw);

public:
	property<DTO(MAC)> Raw{
			_get
			{
				return _dto;
			},
			_set
			{
				_dto = value;
			}
	};

public:
	explicit operator std::string_view() const;

	MAC& operator=(const MAC& rhs);

	bool operator==(const MAC& rhs) const;

	bool operator!=(const MAC& rhs) const;

	uint8_t operator[](size_t i) const;

public:
	static std::optional<MAC> Self(const std::string_view& interface);
	static std::optional<MAC> Self();

public:
	inline static DTO(MAC) UnknownDTO = { 0, 0, 0, 0, 0, 0 };
	inline static DTO(MAC) BroadcastDTO = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

	static MAC Unknown;
	static MAC Broadcast;
};

constexpr bool std::less<MAC>::operator()(const MAC& x, const MAC& y) const
{
	for (size_t i = 0; i < 6; ++i)
		if (x._dto.seg[i] < y._dto.seg[i])
			return true;
		else if (x._dto.seg[i] > y._dto.seg[i])
			return false;
	return true;
}


#endif //ARPCTL_MAC_H
