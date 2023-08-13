#include "net/network_error.h"

network_error::network_error(std::string msg) : _msg(msg)
{
}

const char* network_error::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW
{
	return _msg.data();
}
