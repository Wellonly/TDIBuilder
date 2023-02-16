#include "devicedata.h"
#include "devicedataplugin.h"
#include "debuginfo.h"

#include <QtPlugin>

DeviceDataPlugin::DeviceDataPlugin(QObject *parent)
: QObject(parent)
{
    m_initialized = false;
}

void DeviceDataPlugin::initialize(QDesignerFormEditorInterface* core)
{
    if (m_initialized) return;

    //TODO: find out why Qt Creator has topLevel() and Qt Designer has not topLevel()!!!
    if (Debug::Connect(core->topLevel()))
        Debug::addInfo(core->topLevel()->windowTitle() + " connected: ", this, core);

    m_initialized = true;
}

bool DeviceDataPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *DeviceDataPlugin::createWidget(QWidget *parent)
{
    Debug::addInfo(this, parent);
    if (parent && parent->parent() && !qobject_cast<TDIItem*>(parent)) return new QWidget(parent); //exclude from...
    return new DataItem(parent);
}

QString DeviceDataPlugin::name() const
{
    return QLatin1String("DataItem");
}

QString DeviceDataPlugin::group() const
{
    return QLatin1String("TDI");
}

QIcon DeviceDataPlugin::icon() const
{
    return QIcon(QLatin1String(":/icons/data.png"));
}

QString DeviceDataPlugin::toolTip() const
{
    return QLatin1String("Device data item");
}

QString DeviceDataPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool DeviceDataPlugin::isContainer() const
{
    return false;
}

QString DeviceDataPlugin::domXml() const
{
    return QLatin1String("<widget class=\"DataItem\" name=\"data_1\">\n</widget>\n");
}

QString DeviceDataPlugin::includeFile() const
{
    return QLatin1String("devicedata.h");
}

