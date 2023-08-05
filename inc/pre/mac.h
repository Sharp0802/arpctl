#ifndef ARPCTL_MAC_H
#define ARPCTL_MAC_H

#include "framework.h"
#include "convention.h"
#include "property.h"


decl_DTO(MAC)
{
	uint8_t seg[6];
};

class MAC final
{
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

	uint8_t operator[](size_t i) const;

public:
	inline static DTO(MAC) Unknown = { 0, 0, 0, 0, 0, 0 };
	inline static DTO(MAC) Broadcast = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
};


#endif //ARPCTL_MAC_H
