#include "service/application.h"
#include "service/runopt.h"

/*
 * syntax) arpctl [OPTIONS]... -x [<sender-ip> <target-ip>]...
 *
 * -C : Use infection checker (most of the windows doesn't support ping)
 * -i : Interface device to use (e.g. wlan0, ens0, eth0...)
 * -S : Enable sniffer (default is disabled)
 * -t : Packet response timeout (in milliseconds)
 * -x : Start ip pairs
 *
 * e.g. arpctl -t 1000 -CSi wlan0 -x 192.168.xx.yy 192.168.zz.ww ...
 * -or- arpctl -t 1000 -i wlan0 -CSx 192.168.xx.yy 192.168.zz.ww ...
 *
 * */

bool Application::Configure(const std::vector<std::string_view>& argv)
{
	LOG(VERB) << "parsing arguments...";

	DTO(RunOption) opt{};

	uint8_t mode = 0;
	std::string_view dev;
	std::optional<std::chrono::milliseconds> timeout;

	size_t i = 0;
	for (; argv[i] != "-x" && i < argv.size(); ++i)
	{
		if (argv[i][0] == '-')
		{
			mode = 0;

			if (argv[i].size() <= 1)
			{
				LOG(WARN) << "option expected after '-', not whitespace";
				continue;
			}

			for (size_t j = 1; j < argv[i].size(); ++j)
			{
				mode = 0;
				switch (argv[i][j])
				{
				case 'C':
					opt.chk_icmp = true;
					break;
				case 'i':
					mode = 1;
					break;
				case 'S':
					opt.use_sniffer = true;
					break;
				case 't':
					mode = 3;
					break;
				case 'x':
					mode = 2;
					break;
				}
			}
		}
		else if (mode == 1)
		{
			dev = argv[i];
		}
		else if (mode == 3)
		{
			timeout = std::chrono::milliseconds(std::strtoul(argv[i].data(), nullptr, 10));
		}
	}

	if (dev.empty())
	{
		LOG(CRIT) << "interface name required";
		return false;
	}
	if (argv[i] != "-x" && mode != 2)
	{
		LOG(CRIT) << "ip-pair required";
		return false;
	}
	if (!timeout)
	{
		LOG(CRIT) << "timeout required";
		return false;
	}

	if (mode != 2)
		i++;

	if ((argv.size() - i + 1) % 2)
	{
		LOG(CRIT) << "ip could not be paired";
		return false;
	}

	bool fail = false;
	std::vector<std::shared_ptr<NetworkObject>> set;
	for (; i < argv.size(); ++i)
	{
		auto ip = IP::From(argv[i]);
		if (!ip)
		{
			LOG(FAIL) << "could not parse ip string: " << argv[i];
			fail = true;
			continue;
		}

		auto ptr = std::make_shared<NetworkObject>(ip.value());
		set.push_back(ptr);
	}
	if (fail)
	{
		LOG(CRIT) << "could not load ip-pair";
		return false;
	}

	_obj = set;
	std::vector<std::shared_ptr<NetworkFlow>> flow;
	for (; i < argv.size(); i += 2)
	{
		flow.push_back(std::make_shared<NetworkFlow>(*set.at(i), *set.at(i + 1)));
	}

	_flow = flow;

	LOG(VERB) << "bootstrapping...";

	_bt = std::make_shared<Bootstrapper>(dev, timeout.value());
	if (!_bt->Start())
	{
		LOG(CRIT) << "could not boot";
		return false;
	}

	RunOption _(opt);

	LOG(NOTE) << "application configured!";

	return true;
}

void Application::Start()
{
	/* INITIALIZATION */
	std::vector<std::future<bool>> when;
	when.reserve(_flow.size());
	for (auto& flow: _flow)
		when.emplace_back(flow->InitializeComponentsAsync());

	bool init = true;
	for (auto& task: when)
		init &= task.get();

	when.clear();

	if (!init)
	{
		LOG(CRIT) << "failed to initialize network-flow";
		return;
	}

	/* START FLOW */
	for (auto& flow: _flow)
		when.emplace_back(flow->Run());

	init = true;
	for (auto& task: when)
		init &= task.get();

	when.clear();

	if (!init)
	{
		LOG(CRIT) << "failed to run network-flow";
		return;
	}

	/* BIND RELAYER */
	for (auto& flow: _flow)
		when.emplace_back(flow->Relay());

	init = true;
	for (auto& task: when)
		init &= task.get();

	when.clear();

	if (!init)
	{
		LOG(CRIT) << "failed to initialize relayer";
		return;
	}

	/* START PERIODIC INFECTION */
	auto period = RunOption::Global().Timeout.Get();
	for (auto& flow: _flow)
		flow->Periodic(period);
}

enum Worker::State Application::Join()
{
	auto timeout = RunOption::Global().Timeout.Get();
	for (auto& flow: _flow)
	{
		auto status = flow->Join(timeout);
		if (status == std::future_status::timeout)
		{
			return Worker::State::OCCUPIED;
		}
	}
	return Worker::State::COMPLETED;
}

void Application::Abort()
{
	for (auto& flow: _flow)
		flow->Stop();
}
