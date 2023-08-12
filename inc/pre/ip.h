#ifndef ARPCTL_IP_H
#define ARPCTL_IP_H

#include "framework.h"
#include "convention.h"
#include "property.h"


decl_DTO(IP)
{
	uint8_t seg[4];
};

class IP;

namespace std
{
	template<>
	struct less<IP>
	{
		constexpr bool operator()(const IP& x, const IP& y) const;
	};
}

class IP final
{
	friend struct std::less<IP>;

public:
	enum class Class : uint8_t
	{
		A = 127,
		B = 191,
		C = 223,
		D = 239,
		E = 255
	};

private:
	DTO(IP) _dto;

public:
	IP();

	IP(const IP& rhs);

	explicit IP(DTO(IP) dto);

	explicit IP(void* raw);

	[[nodiscard]]
	static std::optional<IP> From(const std::string_view& str);

	[[nodiscard]]
	static std::optional<IP> Self(const std::string_view& interface);

	[[nodiscard]]
	static std::optional<IP> Self();

public:
	property<DTO(IP)> Raw{
			_get
			{
				return _dto;
			},
			_set
			{
				_dto = value;
			}
	};

	readonly<Class> Class{
			_get
			{
				if (_dto.seg[0] <= static_cast<uint8_t>(Class::A))
					return Class::A;
				if (_dto.seg[0] <= static_cast<uint8_t>(Class::B))
					return Class::B;
				if (_dto.seg[0] <= static_cast<uint8_t>(Class::C))
					return Class::C;
				if (_dto.seg[0] <= static_cast<uint8_t>(Class::D))
					return Class::D;
				return Class::E;
			}
	};

public:
	explicit operator std::string_view() const;

	IP& operator=(const IP& rhs);

	bool operator ==(const IP& rhs) const;

	bool operator !=(const IP& rhs) const;

	uint8_t operator[](size_t i) const;
};

constexpr bool std::less<IP>::operator()(const IP& x, const IP& y) const
{
	for (size_t i = 0; i < 4; ++i)
		if (x._dto.seg[i] < y._dto.seg[i])
			return true;
		else if (x._dto.seg[i] > y._dto.seg[i])
			return false;
	return true;
}


#endif //ARPCTL_IP_H
