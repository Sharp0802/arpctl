#ifndef ARPCTL_BOOTSTRAPPER_H
#define ARPCTL_BOOTSTRAPPER_H

#include "net/transmitter.h"
#include "net/receiver.h"
#include "xthread.h"


class Bootstrapper final
{
private:
	inline static Bootstrapper* _instance;

private:
	std::chrono::milliseconds _timeout;

	Worker* _txw = nullptr;
	Worker* _rxw = nullptr;

	Transmitter* _tx = nullptr;
	Receiver* _rx = nullptr;
	bool _init;

	std::string _dev;

public:
	explicit Bootstrapper(std::string device, std::chrono::milliseconds timeout);

	~Bootstrapper();

	static Bootstrapper& GetInstance();

public:
	bool Start();

	void Stop();

	void Abort();

	readonly<std::string> Device{
			_get
			{
				return _dev;
			}
	};
};


#endif //ARPCTL_BOOTSTRAPPER_H
