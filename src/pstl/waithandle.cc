#include "pstl/waithandle.h"


pstl::waithandle::waithandle()
{
	_sync.lock();
}

pstl::waithandle::~waithandle()
{
	_sync.unlock();
}

void pstl::waithandle::wait()
{
	_sync.lock();
	_sync.unlock();
}

void pstl::waithandle::notify()
{
	_sync.unlock();
}
