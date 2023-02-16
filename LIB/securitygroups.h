#ifndef SECURITYGROUPS_H
#define SECURITYGROUPS_H

#include "linkeddata.h"
#include "zvvDataTypes.h"
#include <QWidget>

class SecurityGroups : public LinkedData
{
    Q_OBJECT
    Q_PROPERTY(Security Group1_Users      READ rightsUser      WRITE setRightsUser)
    Q_PROPERTY(Security Group2_PowerUsers READ rightsPowerUser WRITE setRightsPowerUser)
    Q_PROPERTY(Security Group3_Owners     READ rightsOwner     WRITE setRightsOwner)
    Q_PROPERTY(Security Group4_Tuners     MEMBER m_rightsTuner)
public:

    explicit SecurityGroups(QWidget* parent = 0);
    SecurityGroups(const SecurityGroups& other);
    ~SecurityGroups();

    enum Security {
        NoAccess  = 0,
        ReadOnly  = 1,
        ReadWrite = 2,
        Invisible = 3
    };
    Q_ENUM(Security)

    Security rightsUser() const;
    void setRightsUser(const Security &rightsUser);

    Security rightsPowerUser() const;
    void setRightsPowerUser(const Security &rightsPowerUser);

    Security rightsOwner() const;
    void setRightsOwner(const Security &rightsOwner);

    static byte SecByte(SecurityGroups* instance);

    Security rightsTuner() const;

private:
    Security m_rightsUser;
    Security m_rightsPowerUser;
    Security m_rightsOwner;
    Security m_rightsTuner;
};
Q_DECLARE_METATYPE(SecurityGroups)
//Q_DECLARE_OPAQUE_POINTER(SecurityGroups*)
Q_DECLARE_METATYPE(SecurityGroups*)

#endif // SECURITYGROUPS_H
