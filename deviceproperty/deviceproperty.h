#ifndef DEVICEPROPERTY_H
#define DEVICEPROPERTY_H

#include "deviceitem.h"

class PropertyItem : public TDIItem
{
    Q_OBJECT

public:
    PropertyItem(QWidget *parent = 0);
    PropertyItem(const PropertyItem& other);
    ~PropertyItem();

protected:

private slots:

private:

};
Q_DECLARE_METATYPE(PropertyItem)
Q_DECLARE_METATYPE(PropertyItem*)
//Q_DECLARE_OPAQUE_POINTER(PropertyItem*)


#endif
