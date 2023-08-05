#ifndef ARPCTL_APPLICATION_H
#define ARPCTL_APPLICATION_H

#include "framework.h"
#include "xthread.h"


class Application final
{
private:


public:
	explicit Application(const std::vector<std::string_view>& argv);

	~Application();

	void Start();

	Worker::State Join();

	void Abort();
};


#endif //ARPCTL_APPLICATION_H
