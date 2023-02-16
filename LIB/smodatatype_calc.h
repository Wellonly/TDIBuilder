#ifndef SMODATATYPE_CALC_H
#define SMODATATYPE_CALC_H

#include <stdint.h>
#include "smodatatypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//typedef enum SmoDataType SmoDataType;

enum encoding_extender
{ //extender for dwarf_type enum from dwarf2.h
    DW_ATE_struct  = 0xE0,
    DW_ATE_union   = 0xE1,
    DW_ATE_enum    = 0xE2,

    DW_ATE_pointer = 0x100, /* must be last member and may be or'ed with other encodings. see SmoDataType_calc() */
};

uint8_t SmoDataType_calc(int dataSizeb, int encoding);
uint8_t TdiDataType_calc(int dataSizeb, int encoding);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // SMODATATYPE_CALC_H
