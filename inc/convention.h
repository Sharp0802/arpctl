#ifndef ARPCTL_CONVENTION_H
#define ARPCTL_CONVENTION_H

#define DTO(sym) ___DTO__##sym##_

#define decl_DTO(sym) struct __attribute__((__aligned__(1))) DTO(sym) final

#define CCT(a, b) a##b
#define PAD(n) uint8_t CCT(___dummy_,__COUNTER__)[n];

#endif //ARPCTL_CONVENTION_H
