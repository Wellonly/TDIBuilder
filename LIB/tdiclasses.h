#ifndef TDICLASSES_H
#define TDICLASSES_H
#include <QObject>
#include <stdint.h>
#include "c_cpp_macros.h"
#include "zvvDataTypes.h"

typedef QMap<QString, u8> SmoDataType_map_t;

class TdiLib : public QObject
{
    Q_OBJECT
public:
    TdiLib() {}
    virtual ~TdiLib() {}

    static uint8_t DataType_onTypename(QString type_name);

};


class SmoRecord : public QObject
{
    Q_OBJECT
public:
    SmoRecord(QObject* p = 0) : QObject(p) {}
    SmoRecord(const SmoRecord& o):QObject(o.parent()) {}
    ~SmoRecord() {}

#include "smodatatypes.h"
    Q_ENUM(SmoDataType)
    typedef enum SmoDataType SmoDataType;

    enum NameLengthOption
    {
        Auto    = -3,
        Auto24  = -2,
        Auto8   = -1,
        Fixed0  = 0,
        Fixed1  = 1,
        Fixed2  = 2,
        Fixed3  = 3,
        Fixed4  = 4,
        Fixed5  = 5,
        Fixed6  = 6,
        Fixed7  = 7,
        Fixed8  = 8,
        Fixed9  = 9,
        Fixed10 = 10,
        Fixed11 = 11,
        Fixed12 = 12,
        Fixed13 = 13,
        Fixed14 = 14,
        Fixed15 = 15,
        Fixed16 = 16,
        Fixed17 = 17,
        Fixed18 = 18,
        Fixed19 = 19,
        Fixed20 = 20,
        Fixed21 = 21,
        Fixed22 = 22,
        Fixed23 = 23,
        Fixed24 = 24,
    };
    Q_ENUM(NameLengthOption)

#include "smoheadtypes.h"
    Q_ENUM(SMOHeadTypes)

}; //class SmoRecord : public QObject
Q_DECLARE_METATYPE(SmoRecord)
Q_DECLARE_METATYPE(SmoRecord*)

typedef SmoRecord::SmoDataType SmoDataType;
typedef SmoRecord::NameLengthOption NameLengthOptions;
typedef SmoRecord::SMOHeadTypes SMOHeadTypes;

#endif // TDICLASSES_H



