#include "service/bootstrapper.h"

Bootstrapper::Bootstrapper(std::string device, std::chrono::milliseconds timeout): _timeout(timeout), _dev(device)
{
	_instance = this;

	std::array<char, PCAP_ERRBUF_SIZE> err{};
	std::shared_ptr<pcap_t> pcap(
			pcap_open_live(device.data(), BUFSIZ, 1, 1, err.data()),
			[](pcap_t* p) { pcap_close(p); }
	);
	if (pcap == nullptr)
	{
		LOG(CRIT) << "could not open device (" << err.data() << ')';
		_init = false;
		return;
	}

	_tx = ::typed_alloc<Transmitter>(pcap);
	_rx = ::typed_alloc<Receiver>(pcap);
	_init = true;
}

Bootstrapper::~Bootstrapper()
{
	delete _txw;
	delete _rxw;
	delete _tx;
	delete _rx;
}

Bootstrapper& Bootstrapper::GetInstance()
{
	return *_instance;
}

bool Bootstrapper::Start()
{
	if (!_init)
		return false;

	_txw = ::typed_alloc<Worker>([this](const volatile bool* token) {
		_tx->Run(token);
	});
	_rxw = ::typed_alloc<Worker>([this](const volatile bool* token) {
		_rx->Run(token);
	});

	return true;
}

void Bootstrapper::Stop()
{
	if (_txw) _txw->Stop();
	if (_rxw) _rxw->Stop();
}

void Bootstrapper::Abort()
{
	std::array<uint8_t, 2> iter{ 0, 1 };
	std::for_each(std::execution::par_unseq, iter.begin(), iter.end(), [this](const uint8_t& i) {
		if (i == 0 && _txw && _txw->Join(_timeout) == std::future_status::timeout)
			_txw->Abort();
		if (i == 1 && _rxw && _rxw->Join(_timeout) == std::future_status::timeout)
			_rxw->Abort();
	});
}
