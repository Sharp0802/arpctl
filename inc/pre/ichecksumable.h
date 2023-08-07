#ifndef ARPCTL_ICHECKSUMABLE_H
#define ARPCTL_ICHECKSUMABLE_H


class IChecksumable
{
public:
	virtual uint16_t CalculateChecksum() noexcept = 0;
};


#endif //ARPCTL_ICHECKSUMABLE_H
