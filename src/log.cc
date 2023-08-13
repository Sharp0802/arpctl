#include "pch.h"
#include "log.h"
#include "xthread.h"

msg_body::msg_body(const std::string& heading, std::basic_ostream<char>& out) : _out(out), _buf()
{
	TRACE;
	_buf << '[' << Worker::ID() << ']' << heading;
}

msg_body::~msg_body()
{
	TRACE;
	_buf << std::endl;
	_out << _buf.str();
}
