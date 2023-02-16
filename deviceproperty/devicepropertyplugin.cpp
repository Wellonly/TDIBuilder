#include "deviceproperty.h"
#include "devicepropertyplugin.h"
#include "debuginfo.h"

#include <QtPlugin>

DevicePropertyPlugin::DevicePropertyPlugin(QObject *parent)
: QObject(parent)
{
    m_initialized = false;
}

void DevicePropertyPlugin::initialize(QDesignerFormEditorInterface* core)
{
    if (m_initialized) return;

    //TODO: find out why Qt Creator has topLevel() and Qt Designer has not topLevel()!!!
    if (Debug::Connect(core->topLevel()))
        Debug::addInfo(core->topLevel()->windowTitle() + " connected: ", this, core);

    m_initialized = true;
}

bool DevicePropertyPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *DevicePropertyPlugin::createWidget(QWidget *parentPtr)
{
    Debug::addInfo(this, parentPtr);
    if (parentPtr && parentPtr->parent() && !qobject_cast<TDIItem*>(parentPtr)) return new QWidget(parentPtr); //exclude from...
    return new PropertyItem(parentPtr);
}

QString DevicePropertyPlugin::name() const
{
    return QLatin1String("PropertyItem");
}

QString DevicePropertyPlugin::group() const
{
    return QLatin1String("TDI");
}

QIcon DevicePropertyPlugin::icon() const
{
    return QIcon(QLatin1String(":/icons/property.png"));
}

QString DevicePropertyPlugin::toolTip() const
{
    return QLatin1String("Device property item");
}

QString DevicePropertyPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool DevicePropertyPlugin::isContainer() const
{
    return false;
}

QString DevicePropertyPlugin::domXml() const
{
    return QLatin1String("<widget class=\"PropertyItem\" name=\"property\">\n</widget>\n");
}

QString DevicePropertyPlugin::includeFile() const
{
    return QLatin1String("deviceproperty.h");
}

