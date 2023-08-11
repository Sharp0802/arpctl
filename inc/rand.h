#ifndef ARPCTL_RAND_H
#define ARPCTL_RAND_H

#include "framework.h"
#include "xthread.h"


class Random
{
private:
	static thread_local size_t _seg;

public:
	template<typename T>
	static T Generate()
	{
		_seg ^= std::rotl(_seg, Worker::ID());
		_seg ^= std::rotl(_seg, 11);
		return _seg;
	}
};


#endif //ARPCTL_RAND_H
