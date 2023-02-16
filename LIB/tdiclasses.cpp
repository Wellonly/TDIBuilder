#include "tdiclasses.h"
#include "zvvlib.h"

static const SmoDataType_map_t SmoDataTypeMap = EnumItemsMap<SmoRecord, u8>("SmoDataType");

uint8_t TdiLib::DataType_onTypename(QString type_name)
{
    if (type_name.right(2) == "_t") {
        return SmoDataTypeMap.value(type_name.left(type_name.length()-2), SmoRecord::SmoDataType::Undefined);
    }
    return SmoRecord::SmoDataType::Undefined;
}
