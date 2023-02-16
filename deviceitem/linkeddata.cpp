
#include "linkeddata.h"
#include "debuginfo.h"

#include <QDesignerFormWindowInterface>

LinkedData::LinkedData(QWidget *parent)
: QWidget(parent)
,m_smoDataType(SmoDataType::Undefined)
{

}

LinkedData::LinkedData(const LinkedData &other)
: QWidget(other.parentWidget())
{
    m_smoDataType = other.m_smoDataType;
    m_linkAddress = other.m_linkAddress;
}

LinkedData::~LinkedData()
{
}

QString LinkedData::linkAddress() const
{
//    Debug::addInfo("::linkAddress(getter): ", this, this->parent(), sender());
    return m_linkAddress;
}

void LinkedData::setLinkAddress(const QString &linkAddress)
{
    m_linkAddress = linkAddress;
}
SmoDataType LinkedData::smoDataType() const
{
    return m_smoDataType;
}

void LinkedData::setSmoDataType(SmoDataType smoDataType)
{
    m_smoDataType = smoDataType;
}




