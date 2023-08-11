#ifndef ARPCTL_OCTET_STREAM_H
#define ARPCTL_OCTET_STREAM_H

#include "framework.h"


class OctetStream final
{
private:
	size_t _cap;
	size_t _size;
	uint8_t* _data;

public:
	OctetStream() noexcept;

	OctetStream(const void* dat, size_t s) noexcept;

	OctetStream(OctetStream&& rhs) noexcept;

	OctetStream(const OctetStream& rhs) = delete;

	~OctetStream();

private:
	uint8_t* Release() noexcept;

public:
	void Swap(OctetStream& stream) noexcept;

	[[nodiscard]]
	size_t Capacity() const noexcept;

	[[nodiscard]]
	size_t Size() const noexcept;

	[[nodiscard]]
	uint8_t* Get() const noexcept;

	template<typename T>
	[[nodiscard]]
	T* __As(size_t offset) const noexcept
	{
		return (offset + sizeof(T) > _size)
			   ? nullptr
			   : reinterpret_cast<T*>(_data + offset);
	}

public:
	OctetStream& operator=(const OctetStream& rhs) = delete;

	OctetStream& operator=(OctetStream&& rhs) noexcept
	{
		OctetStream(std::move(rhs)).Swap(*this);
		return *this;
	}

	OctetStream& operator+=(const OctetStream& rhs);

	explicit operator std::string_view() const;
};

#define As template __As


#endif //ARPCTL_OCTET_STREAM_H
