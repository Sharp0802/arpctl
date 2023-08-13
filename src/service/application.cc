#include "service/application.h"

#include <memory>
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

bool Application::Configure(const std::vector<std::string>& argv)
{
	LOG(VERB) << "parsing arguments...";
	std::stringstream str;
	str << "accepted: ";
	for (auto& arg: argv)
		str << " '" << arg << "' ";
	LOG(VERB) << str.str();

	DTO(RunOption) opt{};

	uint8_t mode = 0;
	std::string dev;
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
					opt.view_pkt = true;
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
			opt.timeout = timeout.value();
		}
	}
	_device = dev;

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

	_opt = std::make_shared<RunOption>(opt);

	if (mode != 2)
		i++;

	if ((argv.size() - i) % 2)
	{
		LOG(CRIT) << "ip could not be paired";
		return false;
	}

	/* BOOTSTRAPPING */
	LOG(VERB) << "bootstrapping...";

	_bt = std::make_shared<Bootstrapper>(_device, RunOption::Global().Timeout.Get());
	if (!_bt->Start())
	{
		LOG(CRIT) << "could not boot";
		return false;
	}

	/* CREATE NETWORK-OBJECTS */
	bool fail = false;
	auto f = i;
	std::vector<std::shared_ptr<NetworkObject>> set;
	for (; f < argv.size(); ++f)
	{
		auto ip = IP::From(argv[f]);
		if (!ip)
		{
			LOG(FAIL) << "could not parse ip string: " << argv[f];
			fail = true;
			continue;
		}

		auto ptr = std::make_shared<NetworkObject>(ip.value());
		LOG(VERB) << "network-object created (" << static_cast<std::string>(ptr->IP.Get()) << ')';
		set.push_back(ptr);
	}
	if (fail)
	{
		LOG(CRIT) << "could not load ip-pair";
		return false;
	}

	LOG(VERB) << "creating flow...";
	_obj = set;
	std::vector<std::shared_ptr<NetworkFlow>> flow;
	f = i;
	for (; f < argv.size(); f += 2)
	{
		flow.push_back(std::make_shared<NetworkFlow>(*set.at(f - i), *set.at(f - i + 1)));
		LOG(VERB) << "flow created (" << static_cast<std::string>(set.at(f - i)->IP.Get())
				  << " -> " << static_cast<std::string>(set.at(f - i + 1)->IP.Get());
	}

	_flow = flow;
	LOG(NOTE) << "Flow created(" << _flow.size() << ')';

	LOG(NOTE) << "application configured with:\n"
			  << "view-packet: " << _opt->ViewPacket.Get();

	return true;
}

void Application::Start()
{
	/* INITIALIZATION */
	LOG(VERB) << "initializing(" << _flow.size() << ")...";
	std::vector<std::future<bool>> when;
	when.reserve(_flow.size());
	for (auto& flow: _flow)
	{
		when.emplace_back(flow->InitializeComponentsAsync());
	}

	for (auto& task: when)
		task.wait();

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
	LOG(VERB) << "start flowing...";
	for (auto& flow: _flow)
		when.emplace_back(flow->Run());

	for (auto& task: when)
		task.wait();

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
	LOG(VERB) << "binding relayer...";
	for (auto& flow: _flow)
		when.emplace_back(flow->Relay());

	init = true;
	for (auto& task: when)
		init &= task.get();

	for (auto& task: when)
		task.wait();

	when.clear();

	if (!init)
	{
		LOG(CRIT) << "failed to initialize relayer";
		return;
	}

	/* START PERIODIC INFECTION */
	LOG(VERB) << "start periodic infection...";
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
