#include <sys/ioctl.h>
#include <net/if.h>
#include "pch.h"
#include "pre/mac.h"
#include "service/bootstrapper.h"


MAC MAC::Unknown { &MAC::UnknownDTO };
MAC MAC::Broadcast { &MAC::BroadcastDTO };

[[nodiscard]]
static consteval std::array<char, 16> GetHexTable()
{
	std::array<char, 16> buf{};
	for (size_t i = 0; i < 10; ++i)
		buf[i] = i + '0';
	for (size_t i = 0; i < 6; ++i)
		buf[i + 10] = i + 'A';

	return buf;
}

[[gnu::access(write_only, 1)]]
static void DumpHex(char* dst, uint8_t src)
{
	std::array<char, 16> vt = GetHexTable();
	dst[0] = vt[src & 0xF0 >> 4];
	dst[1] = vt[src & 0x0F >> 0];
}

MAC::MAC() : _dto({ 0, 0, 0, 0, 0, 0 })
{
}

MAC::MAC(const MAC& rhs)
{
	_dto = rhs._dto;
}

MAC::MAC(void* raw)
{
	std::memcpy(&_dto, raw, sizeof(DTO(MAC)));
}

MAC::operator std::string_view() const
{
	std::array<char, 17> buffer{};
	for (size_t i = 0; i < 6; ++i)
		DumpHex(&buffer[3 * i], _dto.seg[i]);
	for (size_t i = 0; i < 5; ++i)
		buffer[i * 3 + 2] = ':';
	return { std::string(buffer.data()) }; // Using raw buffer causes returning temporary variable
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

std::optional<MAC> MAC::Self(const std::string_view& interface)
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
	std::memcpy(&mac, ifr.ifr_hwaddr.sa_data, 6);

	_self.emplace(interface, mac);

	return MAC(&mac);
}

std::optional<MAC> MAC::Self()
{
	auto interface = Bootstrapper::GetInstance().Device.Get();
	return Self(interface);
}
