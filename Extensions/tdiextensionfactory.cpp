#include "tdiextensionfactory.h"

#include <QDesignerPropertySheetExtension>
#include <devicefolder.h>
#include <deviceroot.h>
#include <algorithm>
#include "debuginfo.h"

TDITaskMenu::TDITaskMenu(QObject* factory, TDIItem *deviceItem, QObject *menuParent)
:QObject(menuParent)
,m_deviceItem(deviceItem)
,m_factory(qobject_cast<TDITaskMenuExtensionFactory*>(factory))
{
    menuActions.append(new TDITaskMenuAction(QStringLiteral("Link a data..."), this, TDITaskMenu::SelectLinkToAddress));
//    menuActions.append(new TDITaskMenuAction(QStringLiteral("Link address parameter..."), this, TDITaskMenu::SelectLinkToParameter));
    if (qobject_cast<FolderItem*>(m_deviceItem)) {
        menuActions.append(new TDITaskMenuAction(QStringLiteral("Link child with item..."),this, TDITaskMenu::SelectTDIItemToChildAddress));
        menuActions.append(new TDITaskMenuAction(QStringLiteral("Link child with symbol..."),this, TDITaskMenu::SelectSymbolLinkToChildAddress));
    }
    else if (qobject_cast<DeviceRoot*>(m_deviceItem)) {
        menuActions.append(new TDITaskMenuAction(QStringLiteral("Device data source..."), this, TDITaskMenu::SelectDeviceDataSource));
        menuActions.append(new TDITaskMenuAction(QStringLiteral("Link child with symbol..."),this, TDITaskMenu::SelectSymbolLinkToChildAddress));
        menuActions.append(new TDITaskMenuAction(QStringLiteral("Device header..."),this, TDITaskMenu::SelectDeviceHeader));
    }
}

QAction *TDITaskMenu::preferredEditAction() const
{
    return menuActions.at(0);
}

QList<QAction *> TDITaskMenu::taskActions() const
{
    return menuActions;
}

void TDITaskMenu::SelectLinkToAddress(TDITaskMenu *menu)
{
    SelectLinkTo(menu, SymbolsTreeMap::SelectLinkMode::Data);
}
//void TDITaskMenu::SelectLinkToParameter(TDITaskMenu *menu)
//{
//    SelectLinkTo(menu, SymbolsTreeMap::SelectLinkMode::Parameter);
//}
void TDITaskMenu::SelectSymbolLinkToChildAddress(TDITaskMenu *menu)
{
    SelectLinkTo(menu, SymbolsTreeMap::SelectLinkMode::ChildAddress);
}

void TDITaskMenu::SelectTDIItemToChildAddress(TDITaskMenu *menu)
{
    TDIMap* tdiMap;
    if (menu->factory() && menu->factory()->tdiMap()) {
        tdiMap = menu->factory()->tdiMap();
    }
    else {
        menu->factory()->setTdiMap(tdiMap = new TDIMap(menu->deviceItem()));
    }
    tdiMap->TDIMapExec(menu->deviceItem());
}

void TDITaskMenu::SelectLinkTo(TDITaskMenu *menu, SymbolsTreeMap::SelectLinkMode mode)
{
    SymbolsTreeMap* tree;
    if (menu->factory() && menu->factory()->symbolsTreeMap()) {
        tree = menu->factory()->symbolsTreeMap();
    }
    else {
        menu->factory()->setSymbolsTreeMap(tree = new SymbolsTreeMap(menu->deviceItem()));
    }
    tree->TreeMapExec(menu->deviceItem(), mode);
}

void TDITaskMenu::SelectDeviceHeader(TDITaskMenu *menu)
{
    TDIItem *item = menu->deviceItem();
    QString fileName = QFileDialog::getOpenFileName(item->core()->topLevel()
                        ,tr("Select device header...")
                        ,QDir::currentPath()
                        ,QStringLiteral("Header files (*.h);; All files (*.*)")
                        );
    if (!fileName.isEmpty()) {
        TDIItem::SetPropertyValue(item, QStringLiteral("DeviceHeader"), fileName);
    }
}

void TDITaskMenu::SelectDeviceDataSource(TDITaskMenu *menu)
{
    TDIItem *item = menu->deviceItem();
    QString fileName = QFileDialog::getOpenFileName(item->core()->topLevel()
                        ,tr("Select device data source...")
                        ,QDir::currentPath()
                        ,QStringLiteral("Elf files (*.elf);; All files (*.*)")
                        );
    if (!fileName.isEmpty()) {
        TDIItem::SetPropertyValue(item, QStringLiteral("DataSource"), fileName);

    }
}
TDITaskMenuExtensionFactory *TDITaskMenu::factory() const
{
    return m_factory;
}

TDIItem *TDITaskMenu::deviceItem() const
{
    return m_deviceItem;
}

/////////////////////////////////////// ExtensionFactory...
TDITaskMenuExtensionFactory::TDITaskMenuExtensionFactory(QDesignerFormEditorInterface *core)
: QExtensionFactory(core->extensionManager())
,m_core(core)
,m_symbolsTreeMap(0)
,m_tdiMap(0)
{
}

TDITaskMenuExtensionFactory::~TDITaskMenuExtensionFactory()
{
    delete m_symbolsTreeMap;
    delete m_tdiMap;
}

QObject *TDITaskMenuExtensionFactory::createExtension(QObject *object, const QString &iid, QObject *extensionsParent) const
{
    TDIItem *item = qobject_cast<TDIItem*>(object);
//    Debug::addInfo(this, iid, object, extensionsParent, (this->parent() ? this->parent() : 0));
    if (item && (iid == Q_TYPEID(QDesignerTaskMenuExtension))) {
        //NOTE: a task menu instance created per item!!!
        return new TDITaskMenu((QObject*)this, item, extensionsParent);
    }
    return 0;
}
TDIMap *TDITaskMenuExtensionFactory::tdiMap() const
{
    return m_tdiMap;
}

void TDITaskMenuExtensionFactory::setTdiMap(TDIMap *tdiMap)
{
    m_tdiMap = tdiMap;
}

void TDITaskMenuExtensionFactory::setSymbolsTreeMap(SymbolsTreeMap *treeMap)
{
    m_symbolsTreeMap = treeMap;
}

SymbolsTreeMap *TDITaskMenuExtensionFactory::symbolsTreeMap() const
{
    return m_symbolsTreeMap;
}

QDesignerFormEditorInterface *TDITaskMenuExtensionFactory::core() const
{
    return m_core;
}


TDITaskMenuAction::TDITaskMenuAction(const QString &text, TDITaskMenu *parent, menuActionMethodPtr method)
:QAction(text, parent)
,m_menu(parent)
,m_method(method)
{
    connect(this, &QAction::triggered, [this](){Action();});
}

TDITaskMenuAction::~TDITaskMenuAction()
{
//    disconnect(this, 0,0,0);
}

void TDITaskMenuAction::Action()
{
    m_method(m_menu);
}

