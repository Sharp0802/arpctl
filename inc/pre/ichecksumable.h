#ifndef ARPCTL_ICHECKSUMABLE_H
#define ARPCTL_ICHECKSUMABLE_H


class IChecksumable
{
public:
	[[nodiscard]]
	virtual uint16_t CalculateChecksum() const noexcept = 0;
};


#endif //ARPCTL_ICHECKSUMABLE_H
