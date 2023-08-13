#include <stacktrace>
#include "pch.h"
#include "log.h"
#include "service/iapplication.h"
#include "service/application.h"
#include "service/transmitter_test_application.h"
#include "service/receiver_test_application.h"

using ApplicationType = Application;
static_assert(std::is_base_of_v<IApplication, ApplicationType>);

int main(int argc, char* argv[])
{
	std::vector<std::string> args{ static_cast<size_t>(argc) - 1 };
	for (ssize_t i = 1; i < argc; ++i)
		args[i - 1] = argv[i];

	enum Worker::State result;

	/*
	try
	*/
	{
		ApplicationType app;
		if (!app.Configure(args))
		{
			LOG(CRIT) << "could not configure application";
			return 1;
		}
		app.Start();
		result = app.Join();
		LOG(INFO) << "application returned with " << static_cast<int>(result);
	}
	/*
	catch (const std::exception& e)
	{
		LOG(CRIT) << "Application throws exception.\n"
					 "Application will be terminated immediately\n"
					 "===== EXCEPTION BEGIN =====\n"
				  << e.what()
				  << "\n===== EXCEPTION END =====";
		result = Worker::State::EXCEPTION;
	}
	*/

	if (result == Worker::State::EXCEPTION)
	{
		LOG(CRIT) << "Application exited with exception";
		return -1;
	}

	return 0;
}
