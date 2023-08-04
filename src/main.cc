#include "pch.h"
#include "log.h"
#include "application.h"

int main(int argc, char* argv[])
{
	std::vector<std::string_view> args{ static_cast<size_t>(argc) };
	for (ssize_t i = 0; i < argc; ++i)
		args.emplace_back(argv[i]);

	application app(args);

	app.start();
	auto result = app.join();
	app.abort();

	if (result == worker::state::EXCEPTION)
	{
		// LOG
		return -1;
	}

	return 0;
}
