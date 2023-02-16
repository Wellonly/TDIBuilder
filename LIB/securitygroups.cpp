#include "securitygroups.h"

SecurityGroups::SecurityGroups(QWidget *parent)
: LinkedData(parent)
,m_rightsUser(Security::ReadOnly)
,m_rightsPowerUser(Security::ReadOnly)
,m_rightsOwner(Security::ReadOnly)
,m_rightsTuner(Security::ReadWrite)
{

}

SecurityGroups::SecurityGroups(const SecurityGroups &other)
: LinkedData(other.parentWidget())
{
    m_rightsUser = other.m_rightsUser;
    m_rightsPowerUser = other.m_rightsPowerUser;
    m_rightsOwner = other.m_rightsOwner;
    m_rightsTuner = other.m_rightsTuner;
}

SecurityGroups::~SecurityGroups()
{
}

SecurityGroups::Security SecurityGroups::rightsOwner() const
{
    return m_rightsOwner;
}

void SecurityGroups::setRightsOwner(const Security &rightsOwner)
{
    m_rightsTuner = m_rightsOwner = rightsOwner;
}

byte SecurityGroups::SecByte(SecurityGroups *instance)
{
    uintptr ret = instance->rightsUser();
    ret |= instance->rightsPowerUser() << 2;
    ret |= instance->rightsOwner()     << 4;
    ret |= instance->rightsTuner()     << 6;
    return (byte)ret;
}
SecurityGroups::Security SecurityGroups::rightsTuner() const
{
    return m_rightsTuner;
}


SecurityGroups::Security SecurityGroups::rightsPowerUser() const
{
    return m_rightsPowerUser;
}

void SecurityGroups::setRightsPowerUser(const Security &rightsPowerUser)
{
    m_rightsOwner =
    m_rightsTuner =
    m_rightsPowerUser = rightsPowerUser;
}

SecurityGroups::Security SecurityGroups::rightsUser() const
{
    return m_rightsUser;
}

void SecurityGroups::setRightsUser(const Security &rightsUser)
{
    m_rightsPowerUser =
    m_rightsOwner =
    m_rightsTuner =
    m_rightsUser = rightsUser;
}
