#ifndef ARPCTL_IAPPLICATION_H
#define ARPCTL_IAPPLICATION_H

#include "framework.h"
#include "xthread.h"


class IApplication
{
public:
	virtual ~IApplication();

	virtual bool Configure(const std::vector<std::string_view>& argv) = 0;

	virtual void Start() = 0;

	virtual enum Worker::State Join() = 0;

	virtual void Abort() = 0;
};


#endif //ARPCTL_IAPPLICATION_H
