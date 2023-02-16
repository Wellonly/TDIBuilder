#include "devicedata.h"

DataItem::DataItem(QWidget *parent)
: TDIItem(parent, QIcon(QLatin1String(":/icons/data.png")))
{
}

DataItem::DataItem(const DataItem &other)
: TDIItem(other.parentWidget(), other.icon())
{

}

DataItem::~DataItem()
{

}

