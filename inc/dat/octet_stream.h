#ifndef ARPCTL_OCTET_STREAM_H
#define ARPCTL_OCTET_STREAM_H

#include "framework.h"


class OctetStream final
{
private:
	size_t _cap;
	size_t _size;
#ifdef UNIQUE_DATA
	uint8_t* _data;
#else
	std::shared_ptr<uint8_t[]> _data;
#endif

public:
	OctetStream() noexcept;

	OctetStream(const void* dat, size_t s) noexcept;

	OctetStream(OctetStream&& rhs) noexcept;

	OctetStream(const OctetStream& rhs) = delete;

	~OctetStream();

private:
#ifdef UNIQUE_DATA
	uint8_t* Release() noexcept;
#endif

public:
#ifdef UNIQUE_DATA
	void Swap(OctetStream& stream) noexcept;
#endif

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
#ifdef UNIQUE_DATA
		return (offset + sizeof(T) > _size)
			   ? nullptr
			   : reinterpret_cast<T*>(_data + offset);
#else
		return (offset + sizeof(T) > _size)
			   ? nullptr
			   : reinterpret_cast<T*>(Get() + offset);
#endif
	}

public:
	OctetStream& operator=(const OctetStream& rhs) = delete;

	OctetStream& operator=(OctetStream&& rhs) noexcept;

	OctetStream& operator+=(const OctetStream& rhs);

	explicit operator std::string() const;
};

#define As template __As


#endif //ARPCTL_OCTET_STREAM_H
