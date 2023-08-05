#ifndef ARPCTL_NETWORK_ERROR_H
#define ARPCTL_NETWORK_ERROR_H

#include "framework.h"


class network_error : public std::exception
{
private:
	std::string_view _msg;

public:
	explicit network_error(std::string_view msg);

public:
	[[nodiscard]]
	const char* what() const noexcept override _GLIBCXX_TXN_SAFE_DYN;
};


#endif //ARPCTL_NETWORK_ERROR_H
