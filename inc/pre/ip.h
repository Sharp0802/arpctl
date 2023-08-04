#ifndef ARPCTL_IP_H
#define ARPCTL_IP_H

#include "framework.h"
#include "convention.h"
#include "property.h"


decl_DTO(IP)
{
	uint8_t seg[4];
};

class IP final
{
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

	explicit IP(void* raw);

	explicit IP(const std::string_view& str);

public:
	property<DTO(IP)> Raw {
		_get {
			return _dto;
		},
		_set {
			_dto = value;
		}
	};

	readonly<Class> Class {
		_get {
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
	IP& operator=(const IP& rhs);

	uint8_t operator[](size_t i);
};


#endif //ARPCTL_IP_H
