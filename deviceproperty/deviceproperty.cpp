#include "deviceproperty.h"

PropertyItem::PropertyItem(QWidget *parent)
: TDIItem(parent, QIcon(QLatin1String(":/icons/property.png")))
{
}

PropertyItem::PropertyItem(const PropertyItem &other)
: TDIItem(other.parentWidget(), other.icon())
{

}

PropertyItem::~PropertyItem()
{
}

