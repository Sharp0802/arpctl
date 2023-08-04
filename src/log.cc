#include "log.h"

msg_body::msg_body(std::string_view heading, std::basic_ostream<char>& out) : _out(out), _buf()
{
	_buf << heading;
}

msg_body::~msg_body()
{
	_buf << std::endl;
	_out << _buf.str();
}
