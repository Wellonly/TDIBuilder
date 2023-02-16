#include "devicefolder.h"
#include "devicefolderplugin.h"
#include "debuginfo.h"

#include <QtPlugin>

DeviceFolderPlugin::DeviceFolderPlugin(QObject *parent)
: QObject(parent)
{
    m_initialized = false;
}

void DeviceFolderPlugin::initialize(QDesignerFormEditorInterface* core)
{
    if (m_initialized) return;

    //TODO: find out why Qt Creator has topLevel() and Qt Designer has not topLevel()!!!
    if (Debug::Connect(core->topLevel()))
        Debug::addInfo(core->topLevel()->windowTitle() + " connected(this,parent,core): ", this, (parent() ? parent(): 0), core);

    m_initialized = true;
}

bool DeviceFolderPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *DeviceFolderPlugin::createWidget(QWidget *parent)
{
    Debug::addInfo(this, parent);
    if (parent && parent->parent() && !qobject_cast<TDIItem*>(parent)) return new QWidget(parent); //exclude from...
    if (parent) { // exclude FolderItem from Designer wisard
        return new FolderItem(parent);
    }
    return new QWidget(parent); //NOTE: do not return a zero!!!
}

QString DeviceFolderPlugin::name() const
{
    return QLatin1String("FolderItem");
}

QString DeviceFolderPlugin::group() const
{
    return QLatin1String("TDI");
}

QIcon DeviceFolderPlugin::icon() const
{
    return QIcon(QLatin1String(":/icons/folder.png"));
}

QString DeviceFolderPlugin::toolTip() const
{
    return QLatin1String("Device folder item");
}

QString DeviceFolderPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool DeviceFolderPlugin::isContainer() const
{
    return true;
}

QString DeviceFolderPlugin::domXml() const
{
    return QLatin1String("<widget class=\"FolderItem\" name=\"folder\">\n</widget>\n");
}

QString DeviceFolderPlugin::includeFile() const
{
    return QLatin1String("devicefolder.h");
}

