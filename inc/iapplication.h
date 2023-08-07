#ifndef ARPCTL_IAPPLICATION_H
#define ARPCTL_IAPPLICATION_H

#include "framework.h"
#include "xthread.h"


class IApplication
{
protected:


public:
	explicit IApplication(const std::vector<std::string_view>& argv);

	~IApplication();

	void Start();

	enum Worker::State Join();

	void Abort();
};


#endif //ARPCTL_IAPPLICATION_H
