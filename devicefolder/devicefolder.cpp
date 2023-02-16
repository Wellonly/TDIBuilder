#include "devicefolder.h"
#include "zvvlib.h"

#include <QtDesigner/QExtensionFactory>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QDesignerPropertySheetExtension>

#include <QDesignerFormWindowToolInterface>
#include <QDesignerPropertyEditorInterface>
#include <formwindowbase_p.h>
#include <QDesignerPropertySheetExtension>

FolderItem::FolderItem(QWidget *parent)
: TDIItem(parent, QIcon(QLatin1String(":/icons/folder.png")))
,m_folderType(FolderTypes::GenericFolder)
,m_childAddress(TDIItem::AutoChildKeyWord)
{
}

FolderItem::FolderItem(const FolderItem &other)
: TDIItem(other.parentWidget(), other.icon())
{
    m_folderType = other.m_folderType;
    m_childAddress = other.m_childAddress;
}

FolderItem::FolderItem::~FolderItem()
{

}

QString FolderItem::childAddress() const
{
    return m_childAddress;
}

void FolderItem::setChildAddress(const QString &childAddress)
{
    m_childAddress = childAddress;
}
FolderTypes FolderItem::folderType() const
{
    return m_folderType;
}

void FolderItem::childAddressReset()
{
    m_childAddress = TDIItem::AutoChildKeyWord;
}
void FolderItem::setFolderType(const FolderTypes &folderType)
{
    m_folderType = folderType;
//next only for ChildAddress auto update:...
    ChildAddressTypes addrtype = TDIItem::ChildAddressType(this->childAddress());
    QString childAddr;
    switch (folderType) {
    case FolderTypes::GenericFolder:
    case FolderTypes::FolderType1:
    case FolderTypes::FolderType2:
    case FolderTypes::FolderType3:
    case FolderTypes::FolderType4:
        if (addrtype != ChildAddressTypes::Auto || addrtype != ChildAddressTypes::SymbolLink) {
            childAddr = QString(TDIItem::AutoChildKeyWord);
            goto update_child_address;
        }
        break;
    case FolderTypes::LinkToFolder:
        if (addrtype != ChildAddressTypes::FirstSibLink || addrtype != ChildAddressTypes::ItemLink) {
            childAddr = "";
            goto update_child_address;
        }
        break;
    case FolderTypes::LinkToDevice:
    case FolderTypes::NetFolder:
    default:
        return;
    }
    return;
update_child_address:
    TDIItem::SetPropertyValue(this, QStringLiteral("ChildAddress"), childAddr);
}


