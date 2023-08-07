#include "pch.h"
#include "log.h"
#include "iapplication.h"

int main(int argc, char* argv[])
{
	std::vector<std::string_view> args{ static_cast<size_t>(argc) };
	for (ssize_t i = 0; i < argc; ++i)
		args.emplace_back(argv[i]);

	enum Worker::State result;

	try
	{
		IApplication app(args);
		app.Start();
		result = app.Join();
		app.Abort();
	}
	catch (const std::exception& e)
	{
		LOG(CRIT) << "IApplication throws exception.\n"
					 "IApplication will be terminated immediately\n"
					 "===== EXCEPTION ====="
				  << e.what();
		result = Worker::State::EXCEPTION;
	}

	if (result == Worker::State::EXCEPTION)
	{
		LOG(CRIT) << "IApplication exited with exception";
		return -1;
	}

	return 0;
}
