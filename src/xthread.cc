#include "pch.h"
#include "xthread.h"

size_t Worker::ID()
{
	static std::atomic<size_t> g{ 0 };
	static thread_local size_t r = g++;
	return r;
}
