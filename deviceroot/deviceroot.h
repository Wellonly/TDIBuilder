#ifndef DEVICEROOT_H
#define DEVICEROOT_H

#include "deviceitem.h"

class DeviceRoot : public TDIItem
{
    Q_OBJECT
    Q_PROPERTY(DeviceTypes DeviceType READ deviceType WRITE setDeviceType)
    Q_PROPERTY(TextCodecs  CharacterSet MEMBER m_characterSet)
    Q_PROPERTY(QString DataSource READ dataSource WRITE setDataSource)
    Q_PROPERTY(QString DeviceHeader READ deviceHeader WRITE setDeviceHeader)
    Q_PROPERTY(QString ChildAddress READ childAddress WRITE setChildAddress RESET childAddressReset)

public:
    DeviceRoot(QWidget *parent = 0);
    DeviceRoot(const DeviceRoot& other);
    ~DeviceRoot();

    enum DeviceTypes {
        GenericDevice = 0,
        DeviceType1   = 1,
        DeviceType2   = 2,
        DeviceType3   = 3,
        DeviceType4   = 4,
        DeviceType5   = 5,
        RouteToDevice = 6,
        NetRouter     = 7,
    };
    Q_ENUM(DeviceTypes)

#include "TextCodecs.h"
    Q_ENUM(TextCodecs)

    QString deviceHeader() const;
    void setDeviceHeader(const QString &deviceHeader);

    QString dataSource() const;
    void setDataSource(const QString &dataSource);

    QString childAddress() const;
    void setChildAddress(const QString &childAddress);

    DeviceTypes deviceType() const;
    void setDeviceType(const DeviceTypes &deviceType);

    TextCodecs characterSet() const;

public slots:
    void selectionChangedSlot();
    void childAddressReset();

protected:

private:
    QString m_deviceHeader;
    QString m_dataSource;

    DeviceTypes m_deviceType;
    QString m_childAddress;

    TextCodecs m_characterSet;

};

Q_DECLARE_METATYPE(DeviceRoot)
Q_DECLARE_METATYPE(DeviceRoot*)

typedef DeviceRoot::DeviceTypes DeviceTypes;
typedef DeviceRoot::TextCodecs TextCodecs;

#endif
