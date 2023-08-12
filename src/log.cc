#include "pch.h"
#include "log.h"
#include "xthread.h"

msg_body::msg_body(std::string_view heading, std::basic_ostream<char>& out) : _pad(), _out(out), _buf()
{
	_buf << '[' << Worker::ID() << ']' << heading;
	_buf.seekp(0, std::ios::end);
	_pad = _buf.tellp();
}

msg_body::~msg_body()
{
	_buf << std::endl;
	_out << _buf.str();
}
