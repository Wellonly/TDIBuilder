#include "deviceroot.h"
#include "devicefolder.h"
#include "debuginfo.h"
//#include "tdi.h"

#include <QtDesigner/QExtensionFactory>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QDesignerPropertySheetExtension>

#include <QDesignerFormWindowToolInterface>
#include <QDesignerPropertyEditorInterface>
#include <formwindowbase_p.h>

DeviceRoot::DeviceRoot(QWidget *parent)
: TDIItem(parent,QIcon(QLatin1String(":/icons/device.png")))
,m_childAddress(TDIItem::AutoChildKeyWord)
,m_characterSet(TextCodecs::UTF_8)
{

}
DeviceRoot::DeviceRoot(const DeviceRoot &other)
:TDIItem(other.parentWidget(), other.icon())
{
    m_deviceHeader = other.m_deviceHeader;
    m_dataSource = other.m_dataSource;
    m_deviceType = other.m_deviceType;
    m_childAddress = other.m_childAddress;
    m_characterSet = other.m_characterSet;
}

DeviceRoot::~DeviceRoot()
{

}

QString DeviceRoot::deviceHeader() const
{
    return m_deviceHeader;
}
void DeviceRoot::setDeviceHeader(const QString &deviceHeader)
{
    m_deviceHeader = deviceHeader;
}

QString DeviceRoot::dataSource() const
{
    return m_dataSource;
}
void DeviceRoot::setDataSource(const QString &dataSource)
{
    m_dataSource = dataSource;
}

void DeviceRoot::selectionChangedSlot()
{
//    QDesignerFormWindowInterface *form = QDesignerFormWindowInterface::findFormWindow(this);
//    if (!form) return;
//    QDesignerFormEditorInterface *editor = form->core();
//    QExtensionManager *manager = editor->extensionManager();
//    QDesignerFormWindowCursorInterface* cursor = form->cursor();
//    QDesignerPropertyEditorInterface* propertyEditor = editor->propertyEditor();
//    typedef qdesigner_internal::FormWindowBase designerFormWindow;
//    designerFormWindow* formw = qobject_cast<designerFormWindow*>(sender());
//    QDesignerFormWindowToolInterface* currtool = formw->tool(formw->currentTool());

//NOTE: currtool->editor() returns: WidgetEditorTool, SignalSlotsEditorTool, TabOrderEditorTool, BuddyEditorTool
//    for (int i=0; i<formw->toolCount(); i++) {
//        Debug::addInfo("DeviceRoot::selectionChangedSlot:tool:"
//                       ,i
//                       ,formw->tool(i)
//                       );
//    }

//    if (FolderItem* folder = qobject_cast<FolderItem*>(cursor->current())) {
//        if (!m_propertySheet) m_propertySheet = qt_extension<QDesignerPropertySheetExtension*>(manager, folder);
//        if (m_propertySheet) {
//            int index = m_propertySheet->indexOf(QLatin1String("ChildAddress"));
//            if (index) {
//                m_propertySheet->setChanged(index, isChildAddressVisible);
////                m_propertySheet->setVisible(index, isChildAddressVisible);
////                m_propertySheet->setChanged(index,true);
//            }
//        }
//    }

    Debug::addInfo("Root::selectionChangedSlot: ",sender());
//                   ,cursor->current()
//                   ,sender()
//                   ,propertyEditor->currentPropertyName()
//                   ,currtool->editor()
    //                   );
}

void DeviceRoot::childAddressReset()
{
    m_childAddress = TDIItem::AutoChildKeyWord;
}
void DeviceRoot::setDeviceType(const DeviceTypes &deviceType)
{
    m_deviceType = deviceType;
//next only for ChildAddress auto update:...
    ChildAddressTypes addrtype = TDIItem::ChildAddressType(this->childAddress());
    QString childAddr;
    switch (deviceType) {
    case DeviceTypes::GenericDevice:
    case DeviceTypes::DeviceType1:
    case DeviceTypes::DeviceType2:
    case DeviceTypes::DeviceType3:
    case DeviceTypes::DeviceType4:
    case DeviceTypes::DeviceType5:
        if (addrtype != ChildAddressTypes::Auto || addrtype != ChildAddressTypes::SymbolLink) {
            childAddr = QString(TDIItem::AutoChildKeyWord);
            goto update_child_address;
        }
        break;
    case DeviceTypes::RouteToDevice:
    case DeviceTypes::NetRouter:
    default:
        return;
    }
    return;
update_child_address:
    TDIItem::SetPropertyValue(this, QStringLiteral("ChildAddress"), childAddr);
}

TextCodecs DeviceRoot::characterSet() const
{
    return m_characterSet;
}

DeviceTypes DeviceRoot::deviceType() const
{
    return m_deviceType;
}

QString DeviceRoot::childAddress() const
{
    return m_childAddress;
}

void DeviceRoot::setChildAddress(const QString &childAddress)
{
    m_childAddress = childAddress;
}

//DeviceRoot::DeviceRoot(const DeviceRoot &other)
//{
//    return other;
//}
