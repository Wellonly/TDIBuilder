#ifndef DEVICEDATA_H
#define DEVICEDATA_H

#include "deviceitem.h"

class DataItem : public TDIItem
{
    Q_OBJECT

public:
    DataItem(QWidget *parent = 0);
    DataItem(const DataItem& other);
    ~DataItem();

protected:

private slots:

private:

};
Q_DECLARE_METATYPE(DataItem)
Q_DECLARE_METATYPE(DataItem*)
//Q_DECLARE_OPAQUE_POINTER(DataItem*)

#endif
