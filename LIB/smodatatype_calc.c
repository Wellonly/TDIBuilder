#include "smodatatype_calc.h"
#include "smodatatypes.h"
#include "c_cpp_macros.h"
#include "dwarf2.h"

//TODO: DateTime64 Decimal64 SizedDataU8 SizedDataU32 DateBSD32 RealTime32 and other types...
//TODO: it upon a array(matrix)

//SmoDataType_calc() returns: SmoDataType::Undefined if the conversion failed or datasize < 0 !!!
uint8_t SmoDataType_calc(int dataSizeb, int encoding)
{
    if (dataSizeb <= 0) return iif(dataSizeb == 0, Void, Undefined);
    if (encoding >= DW_ATE_pointer) {
        if (dataSizeb == 2) return Pointer16;
        if (dataSizeb == 4) return Pointer32;
        if (dataSizeb == 8) return Pointer64;
        goto DTUndefined;
    }
    switch (encoding) {
    case DW_ATE_signed:
        if (dataSizeb <= 2 || dataSizeb == 4)
            return /*(enum SmoDataType)*/ dataSizeb;
        if (dataSizeb == 8)
            return S64;
        break;
    case DW_ATE_unsigned:
        if (dataSizeb <= 2 || dataSizeb == 4)
            return /*(enum SmoDataType)*/ (dataSizeb | smoDT_IsUnsigned_bm);
        if (dataSizeb == 8)
            return U64;
        break;
    case DW_ATE_signed_char:
        if (dataSizeb == 1)
            return S8;
        break;
    case DW_ATE_boolean:
        if (dataSizeb == 1)
            return U8;
        break;
    case DW_ATE_unsigned_char:
        if (dataSizeb <= 127)
            return /*(enum SmoDataType)*/(zeString + dataSizeb);
        break;
    case DW_ATE_float:
        if (dataSizeb == 4) return Float32;
        if (dataSizeb == 8) return Float64;
        break;
    case DW_ATE_struct:
    case DW_ATE_union:
    case DW_ATE_enum:
        if (dataSizeb <= 127) return /*(enum SmoDataType)*/(zeString + dataSizeb);
        if (dataSizeb == 128) return szData128;
        break;
    case DW_ATE_void:
        return Void;
    default:
        break;
    }
DTUndefined:
    return Undefined;
}

//handle exclusives variants for TDI ...
uint8_t TdiDataType_calc(int dataSizeb, int encoding)
{
    if (encoding == DW_ATE_void && dataSizeb > 0) {
        if (dataSizeb == 2) return Pointer16;
        if (dataSizeb == 4) return Pointer32;
        if (dataSizeb == 8) return Pointer64;
    }
    if (encoding == DW_ATE_unsigned_char && dataSizeb == 1) {
        return U8;
    }
    return SmoDataType_calc(dataSizeb, encoding);
}

