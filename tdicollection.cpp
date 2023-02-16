
#include <QDebug>
#include "devicerootplugin.h"
#include "devicefolderplugin.h"
#include "devicedataplugin.h"
#include "devicepropertyplugin.h"
#include "tdicollection.h"

TDICollection::TDICollection(QObject *parent)
    : QObject(parent)
{
    m_widgets.append(new DeviceRootPlugin(this));
    m_widgets.append(new DeviceFolderPlugin(this));
    m_widgets.append(new DevicePropertyPlugin(this));
    m_widgets.append(new DeviceDataPlugin(this));

}

QList<QDesignerCustomWidgetInterface*> TDICollection::customWidgets() const
{
    return m_widgets;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(tdicollectionplugin, TDICollection)
#endif // QT_VERSION < 0x050000
