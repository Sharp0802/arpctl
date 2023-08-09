#ifndef ARPCTL_RECEIVER_TEST_APPLICATION_H
#define ARPCTL_RECEIVER_TEST_APPLICATION_H

#include "framework.h"
#include "log.h"
#include "iapplication.h"
#include "net/receiver.h"


class ReceiverTestApplication : public IApplication
{
private:
	std::string_view _dev;
	std::shared_ptr<pcap_t> _pcap;
	Receiver* _receiver;
	Worker* _worker;

public:
	~ReceiverTestApplication() override = default;

	bool Configure(const std::vector<std::string_view>& argv) override
	{
		if (argv.size() % 2 != 1 || argv.size() < 3)
		{
			LOG(CRIT) << "invalid syntax";
			LOG(NOTE) << "syntax: arpctl <interface>\n"
						 "sample: arpctl wlan0";
			return false;
		}

		_dev = argv[0];

		return true;
	}

	void Start() override
	{
		std::array<char, PCAP_ERRBUF_SIZE> err{};
		std::shared_ptr<pcap_t> pcap(
				pcap_open_live(_dev.data(), BUFSIZ, 1, 1, err.data()),
				[](pcap_t* p) { pcap_close(p); }
		);
		if (pcap == nullptr)
		{
			LOG(FAIL) << "couldn't open device" << _dev << '(' << std::string_view(err) << ")\n";
			return;
		}
		_pcap = pcap;
		_receiver = new Receiver(pcap);

		Receiver::HandlerType handler([](const auto& header, const auto& stream) {
			if (header.GetType() != HeaderSet::SpecialType::ICMP)
				return;

			LOG(VERB) << "Packet captured: Size=" << stream.Size();
		});

		Receiver::GetInstance() += handler;

		_worker = new Worker([this](const volatile bool* token){
			_receiver->Run(token);
		});
		_worker->Start();
	}

	enum Worker::State Join() override
	{
		_worker->Join();
		return _worker->State.Get();
	}

	void Abort() override
	{
		_worker->Abort();
	}
};


#endif //ARPCTL_RECEIVER_TEST_APPLICATION_H
