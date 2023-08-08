#include "pch.h"
#include "dat/octet_stream.h"

OctetStream::OctetStream(const void* dat, size_t s) noexcept
{
	_cap = _size = s;
	_data = new uint8_t[s];
	_rt_memcpy(_data, dat, s);
}

OctetStream::OctetStream(OctetStream&& rhs) noexcept
{
	_cap = rhs._cap;
	_size = rhs._size;
	_data = rhs.Swap();
}

OctetStream::~OctetStream()
{
	delete[] _data;
}

uint8_t* OctetStream::Swap() noexcept
{
	auto old = _data;
	delete[] _data;
	return old;
}

size_t OctetStream::Capacity() const noexcept
{
	return _cap;
}

size_t OctetStream::Size() const noexcept
{
	return _size;
}

uint8_t* OctetStream::Get() const noexcept
{
	return _data;
}

OctetStream& OctetStream::operator+=(const OctetStream& rhs)
{
	if (_cap < _size + rhs._size)
	{
		do
		{
			_cap <<= 1;
		}
		while (_cap < _size + rhs._size);

		auto tmp = new uint8_t[_cap];
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
}
