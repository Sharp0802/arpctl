#ifndef ARPCTL_CONVENTION_H
#define ARPCTL_CONVENTION_H

#define DTO(sym) ___DTO__##sym##_

#define decl_DTO(sym) struct __attribute__((__aligned__(1))) DTO(sym) final

#endif //ARPCTL_CONVENTION_H
