#ifndef ARPCTL_WAITHANDLE_H
#define ARPCTL_WAITHANDLE_H

#include "framework.h"


namespace pstl
{
	class waithandle final
	{
	private:
		std::mutex _sync;

	public:
		waithandle();
		~waithandle();

		waithandle(const waithandle&) = delete;
		waithandle& operator=(const waithandle&) = delete;

	public:
		void wait();
		void notify();
	};
}


#endif //ARPCTL_WAITHANDLE_H
