#include <sys/ioctl.h>
#include <net/if.h>
#include "pch.h"
#include "pre/mac.h"
#include "service/bootstrapper.h"


MAC MAC::Unknown{ &MAC::UnknownDTO };
MAC MAC::Broadcast{ &MAC::BroadcastDTO };

MAC::MAC() : _dto({ 0, 0, 0, 0, 0, 0 })
{
}

MAC::MAC(const MAC& rhs)
{
	_dto = rhs._dto;
}

MAC::MAC(void* raw)
{
	_rt_memcpy(&_dto, raw, sizeof(DTO(MAC)));
}

MAC::operator std::string() const
{
	std::array<char, 18> buffer{};
	sprintf(buffer.data(), "%hhX:%hhX:%hhX:%hhX:%hhX:%hhX",
			_dto.seg[0], _dto.seg[1], _dto.seg[2],
			_dto.seg[3], _dto.seg[4], _dto.seg[5]);
	return { buffer.data() }; // Using raw buffer causes returning temporary variable
}

MAC& MAC::operator=(const MAC& rhs)
{
	Raw = rhs.Raw.Get();
	return *this;
}

bool MAC::operator==(const MAC& rhs) const
{
	for (size_t i = 0; i < 6; ++i)
		if (_dto.seg[i] != rhs._dto.seg[i])
			return false;
	return true;
}

bool MAC::operator!=(const MAC& rhs) const
{
	for (size_t i = 0; i < 6; ++i)
		if (_dto.seg[i] != rhs._dto.seg[i])
			return true;
	return false;
}

uint8_t MAC::operator[](size_t i) const
{
	return _dto.seg[i];
}

std::optional<MAC> MAC::Self(const std::string& interface)
{
	if (_self.contains(interface))
		return MAC(&_self[interface]);

	int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (fd == -1) return std::nullopt;

	struct ifreq ifr{};
	_rt_memcpy(ifr.ifr_name, interface.data(), interface.size());
	ioctl(fd, SIOCGIFHWADDR, &ifr);
	close(fd);

	DTO(MAC) mac{};
	_rt_memcpy(&mac, ifr.ifr_hwaddr.sa_data, 6);

	_self.emplace(interface, mac);

	return MAC(&mac);
}

std::optional<MAC> MAC::Self()
{
	auto interface = Bootstrapper::GetInstance().Device.Get();
	return Self(interface);
}
