#include "deviceroot.h"
#include "devicerootplugin.h"
#include "tdiextensionfactory.h"
#include "debuginfo.h"

#include <QDesignerFormWindowInterface>
#include <QDesignerPropertyEditorInterface>
#include <QDesignerTaskMenuExtension>
#include <QExtensionManager>
#include <QtPlugin>

DeviceRootPlugin::DeviceRootPlugin(QObject *parent)
: QObject(parent)
,m_initialized(false)
,m_connected(false)
,m_core(0)
{
}

void DeviceRootPlugin::initialize(QDesignerFormEditorInterface* core )
{
    if (m_initialized) return;

    m_core = core;
    //TODO: find out why Qt Creator has topLevel() and Qt Designer has not topLevel()!!!
    if (Debug::Connect(core->topLevel()))
        Debug::addInfo(core->topLevel()->windowTitle() + " connected: (this,parent,core) ", this, (parent() ? parent() : 0), core);

    QExtensionManager *manager = core->extensionManager();
    manager->registerExtensions(new TDITaskMenuExtensionFactory(core), Q_TYPEID(QDesignerTaskMenuExtension));

    // Add extension registrations, etc. here...
    m_initialized = true;
}

bool DeviceRootPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *DeviceRootPlugin::createWidget(QWidget *parent)
{
    if (parent && parent->objectName() != "formContainer") return new QWidget(parent); //exclude from ...
    DeviceRoot* root = new DeviceRoot(parent);
    if (!m_connected && parent) {
        QDesignerFormWindowInterface *formWindow = QDesignerFormWindowInterface::findFormWindow(parent);
        if (formWindow) {
            connect(formWindow, SIGNAL(selectionChanged()), root, SLOT(selectionChangedSlot()));
        }

        //NOTE: a reset value doesn't sent propertyChanged signal!!!
        if (QDesignerPropertyEditorInterface *propertyEditor = m_core->propertyEditor()) {
            m_connected = connect(propertyEditor, SIGNAL(propertyChanged(QString, QVariant)),
                                    this, SLOT(slotCheckProperty(QString,QVariant)));
        }
    }
//    Debug::addInfo(this, parent, "connected: ",m_connected);
    return root;
}

void DeviceRootPlugin::slotCheckProperty(const QString &propertyName, const QVariant &newValue)
{
    QDesignerPropertyEditorInterface *propertyEditor = m_core->propertyEditor();
    if (TDIItem *item = qobject_cast<TDIItem*>(propertyEditor->object())) {
        TDIItem::slotPropertyChanged(item, propertyName, newValue);
    }
}

QString DeviceRootPlugin::name() const
{
    return QLatin1String("DeviceRoot");
}

QString DeviceRootPlugin::group() const
{
    return QLatin1String("TDI");
}

QIcon DeviceRootPlugin::icon() const
{
    return QIcon(QLatin1String(":/icons/device.png"));
}

QString DeviceRootPlugin::toolTip() const
{
    return QLatin1String("Device root item");
}

QString DeviceRootPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool DeviceRootPlugin::isContainer() const
{
    return true;
}

QString DeviceRootPlugin::domXml() const
{
    return QLatin1String("<widget class=\"DeviceRoot\" name=\"root\">\n</widget>\n");
}

QString DeviceRootPlugin::includeFile() const
{
    return QLatin1String("deviceroot.h");
}

