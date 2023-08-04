#ifndef ARPCTL_APPLICATION_H
#define ARPCTL_APPLICATION_H

#include "framework.h"
#include "xthread.h"


class application final
{
public:
    explicit application(const std::vector<std::string_view>& argv);
    ~application();

    void start();
    worker::state join();
    void abort();

private:
	void help();
};


#endif //ARPCTL_APPLICATION_H
