#include <mcheck.h>
#include "pch.h"
#include "dat/octet_stream.h"
#include "log.h"

OctetStream::OctetStream() noexcept : _cap(16), _size(0), _data(::typed_alloc_v<uint8_t>(_cap))
{
	TRACE;
}

OctetStream::OctetStream(const void* dat, size_t s) noexcept : _cap(s), _size(s), _data(::typed_alloc_v<uint8_t>(s))
{
	TRACE;
#ifdef UNIQUE_DATA
	_rt_memcpy(_data, dat, s);
#else
	_rt_memcpy(_data.get(), dat, s);
#endif
}

size_t OctetStream::Capacity() const noexcept
{
	TRACE;
	return _cap;
}

size_t OctetStream::Size() const noexcept
{
	TRACE;
	return _size;
}

uint8_t* OctetStream::Get() const noexcept
{
	TRACE;
#ifdef UNIQUE_DATA
	return const_cast<uint8_t*>(_data);
#else
	return const_cast<uint8_t*>(_data.get());
#endif
}

OctetStream::operator std::string() const
{
	TRACE;
	std::vector<char> buf(2 * _size + 1);
	for (size_t i = 0; i < _size; ++i)
		std::snprintf(&buf[i * 2], 3, "%02hX", _data[i]);
	return { buf.data() };
}

#ifdef UNIQUE_DATA

OctetStream::OctetStream(OctetStream&& rhs) noexcept
{
	TRACE;
	_cap = rhs._cap;
	_size = rhs._size;
	_data = rhs.Release();
}

OctetStream::~OctetStream()
{
	TRACE;
	if (mprobe(_data) == MCHECK_OK)
	{
		delete[] _data;
		_data = nullptr;
	}
}

uint8_t* OctetStream::Release() noexcept
{
	TRACE;
	auto old = _data;
	if (mprobe(_data) == MCHECK_OK)
	{
		delete[] _data;
		_data = nullptr;
	}
	return old;
}

void OctetStream::Swap(OctetStream& stream) noexcept
{
	TRACE;
	std::swap(_size, stream._size);
	std::swap(_cap, stream._cap);
	std::swap(_data, stream._data);
}

OctetStream& OctetStream::operator+=(const OctetStream& rhs)
{
	TRACE;
	if (_cap < _size + rhs._size)
	{
		do
		{
			_cap <<= 1;
		}
		while (_cap < _size + rhs._size);

		auto tmp = ::typed_alloc_v<uint8_t>(_cap);
		_rt_memcpy(tmp, _data, _size);
		_rt_memcpy(tmp + _size, rhs._data, rhs._size);

		delete[] _data;
		_data = tmp;
		_size += rhs._size;
	}
	else
	{
		_rt_memcpy(_data + _size, rhs._data, rhs._size);
		_size += rhs._size;
	}

	return *this;
}

OctetStream& OctetStream::operator=(OctetStream&& rhs) noexcept
{
	TRACE;
	OctetStream(std::move(rhs)).Swap(*this);
	return *this;
}

#else //SHARED_DATA

OctetStream::OctetStream(OctetStream&& rhs) noexcept : _cap(rhs._cap), _size(rhs._size), _data(::typed_alloc_v<uint8_t>(rhs._cap))
{
	_rt_memcpy(_data.get(), rhs._data.get(), _size);
}

OctetStream::~OctetStream()
{
}

OctetStream& OctetStream::operator=(OctetStream&& rhs) noexcept
{
	if (_cap < rhs._size)
	{
		std::shared_ptr<uint8_t[]> ptr(::typed_alloc_v<uint8_t>(_cap + rhs._size));
		_rt_memcpy(ptr.get(), _data.get(), _size);
		_data.swap(ptr);

		_cap += rhs._size;
		_size += rhs._size;
	}
	_rt_memcpy(_data.get(), rhs._data.get(), _size);

	return *this;
}

OctetStream& OctetStream::operator+=(const OctetStream& rhs)
{
	if (_cap < rhs._size)
	{
		std::shared_ptr<uint8_t[]> ptr(::typed_alloc_v<uint8_t>(_cap + rhs._size));
		_rt_memcpy(ptr.get(), _data.get(), _size);
		_data.swap(ptr);

		_cap += rhs._size;
	}
	_rt_memcpy(_data.get() + _size, rhs._data.get(), rhs._size);
	_size += rhs._size;

	return *this;
}

#endif
