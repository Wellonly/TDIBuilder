#include "deviceitem.h"
#include "deviceroot.h"
#include "devicefolder.h"
#include "debuginfo.h"

#include <QStylePainter>
#include <QStyleOptionButton>
#include <QEvent>
#include <QDesignerWidgetFactoryInterface>
#include <QDesignerPropertySheetExtension>

#include <QtDesigner/QExtensionManager>
#include <QDesignerPropertyEditorInterface>

#include <QDesignerFormEditorInterface>
#include <QDesignerFormWindowManagerInterface>
#include <QDesignerFormWindowInterface>
#include <QDesignerFormWindowCursorInterface>
#include <QDesignerObjectInspectorInterface>
#include <QtUiPlugin/QDesignerCustomWidgetInterface> //deprecated:<QDesignerCustomWidgetInterface>
#include <layoutinfo_p.h>

//Reserved words used for ChildAddress:
const char* TDIItem::SymbolLinkPrefix    = SymbolLinkPrefixStr; /*NOTE: define also used in sizeof */
const char* TDIItem::AutoChildKeyWord    = "auto";
const char* TDIItem::FirstSibLinkKeyWord = "FirstSiblingLink";
//end Reserved words used for ChildAddress!


TDIItem::TDIItem(QWidget *parent, const QIcon &icon)
:SecurityGroups(parent)
,m_icon(icon)
,childItems(0)
,m_nameLength(NameLengthOptions::Auto)
,m_core(0)
,m_cargo(NULL)
{
//    Debug::addInfo("::init(this,parent,plugin): ", this, (this->parent() ? this->parent(): 0)); //TODO: find out where the plugin pointer is ?
    setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
    connect(this, SIGNAL(objectNameChanged(QString)), this, SLOT(objectNameChangedSlot(QString)));

}

TDIItem::TDIItem(const TDIItem &other)
: SecurityGroups(other.parentWidget())
{
    m_vboxLayout = other.m_vboxLayout;
    m_itemText = other.m_itemText;
    childItems = other.childItems;
    m_nameLength = other.m_nameLength;
    m_cargo = other.m_cargo;
}

TDIItem::~TDIItem()
{
//    disconnect(this, 0, 0, 0);
}

QIcon TDIItem::icon() const
{
    return m_icon;
}
void TDIItem::setIcon(const QIcon &icon)
{
    m_icon = icon;
}

//example: ObjectIspector_update(this);
void TDIItem::ObjectIspector_update(QWidget *widget)
{
    if (QDesignerFormWindowInterface *f = QDesignerFormWindowInterface::findFormWindow(widget)) {
        f->core()->objectInspector()->setFormWindow(f);
    }
}

bool TDIItem::SetPropertyChanged(TDIItem *item, const QString &propertyName, bool value, bool isPropertySheetToBeUpdated)
{
    QDesignerPropertySheetExtension* ext = qt_extension<QDesignerPropertySheetExtension*>(item->core()->extensionManager(), item);
    int index = ext ? ext->indexOf(propertyName) : -2;
    if (index >= 0) {
        ext->setChanged(index, value);
        if (isPropertySheetToBeUpdated) item->core()->propertyEditor()->setObject(item);
    }
    return index >= 0;
}

bool TDIItem::SetPropertyVisible(TDIItem *item, const QString &propertyName, bool value, bool isPropertySheetToBeUpdated)
{
    QDesignerPropertySheetExtension* ext = qt_extension<QDesignerPropertySheetExtension*>(item->core()->extensionManager(), item);
    int index = ext ? ext->indexOf(propertyName) : -2;
    if (index >= 0) {
        ext->setVisible(index, value); //TODO: find out why: ext->setVisible(index, value); doesn't work!!!
        if (isPropertySheetToBeUpdated) item->core()->propertyEditor()->setObject(item);
    }
    return index >= 0;
}

void TDIItem::SetPropertyValue(TDIItem *item, const QString &propertyName, const QVariant& value)
{
    if (QDesignerFormWindowInterface *f = QDesignerFormWindowInterface::findFormWindow(item)) {
        f->cursor()->setProperty(propertyName, value);
    }
}

QString TDIItem::FullPath(TDIItem *item)
{
    QString ret(item->ItemText());
    TDIItem *it;
    for (it = qobject_cast<TDIItem*>(item->parent()); it != NULL; it = qobject_cast<TDIItem*>(it->parent())) {
        ret = it->ItemText() + "/" + ret;
    }
    return ret;
}

///skip non TDIItem objects!!!
TDIItem *TDIItem::ParentItem(TDIItem *item)
{
    TDIItem *it;
    for (it = qobject_cast<TDIItem*>(item->parent()); it != NULL; it = qobject_cast<TDIItem*>(it->parent())) {
        return it;
    }
    return NULL;
}

ChildAddressTypes TDIItem::ChildAddressType(const QString &childAddress)
{
    if (!QString::compare(childAddress, TDIItem::AutoChildKeyWord)) {
        return ChildAddressTypes::Auto;
    }
    else if (!QString::compare(childAddress, TDIItem::FirstSibLinkKeyWord)) {
        return ChildAddressTypes::FirstSibLink;
    }
    else if (childAddress.indexOf(TDIItem::SymbolLinkPrefix) == 0) {
        return ChildAddressTypes::SymbolLink;
    }
    else if (childAddress.indexOf('/') > 0) {
        return ChildAddressTypes::ItemLink;
    }
    else if (childAddress.constData()->isDigit()) {
        if (childAddress.indexOf("0x", 0, Qt::CaseSensitivity::CaseInsensitive) == 0) {
            return ChildAddressTypes::UserDataHex;
        }
        else {
            return ChildAddressTypes::UserDataDec;
        }
    }
//TODO: ChildAddressTypes::DefinedData !!!
    return ChildAddressTypes::ChildAddressUndefined;
}

QDesignerFormEditorInterface *TDIItem::core()
{
    if (!m_core) {
        QDesignerFormWindowInterface *f = QDesignerFormWindowInterface::findFormWindow(this);
        m_core = f->core();
    }
    return m_core;
}

QString TDIItem::ItemText() const
{
    //    Debug::addInfo("::ItemText(getter): ", this, this->parent());
    return m_itemText;
}
void TDIItem::setItemText(const QString &itemText)
{
    m_itemText = itemText;
    update();
}

DeviceRoot* TDIItem::DeviceRootItem(QObject *item)
{
    if (qobject_cast<TDIItem*>(item)) {
        if (qobject_cast<DeviceRoot*>(item)) {
            return (DeviceRoot*)item;
        }
        else {
            return TDIItem::DeviceRootItem(item->parent());
        }
    }
    return NULL; //(DeviceRoot*)0;
}

void TDIItem::NameLengthOption_reset()
{ //TODO: set the reset button active!!!
    if (m_nameLength == NameLengthOptions::Auto) {
        m_nameLength = NameLengthOptions::Auto8;
    }
    else if (m_nameLength == NameLengthOptions::Auto8) {
        m_nameLength = NameLengthOptions::Auto24;
    }
    else {
        m_nameLength = NameLengthOptions::Auto;
    }
//    TDIItem::SetPropertyChanged(this, QStringLiteral("NameLength"), true);
}

QSize TDIItem::sizeHint() const
{
    QSize size;
    size.setWidth((qobject_cast<const DeviceRoot*>(this) ? 311 : 211));
    size.setHeight(totalHeight(this->childItems));
//    Debug::addInfo("sizeHint: ", size, this, "items: ", this->totalItems);
    return size;
}

QSize TDIItem::minimumSizeHint() const
{
    QSize size = sizeHint();
//    Debug::addInfo("minimumSizeHint: ", size, this);
    return size;
}

bool TDIItem::event(QEvent *e)
{
    Debug::addInfo(e->type(), this, this->parent(), sender());
    return QWidget::event(e);
}

//NOTE: this->update() : force calls the paintEvent()!!!
//TODO: optimize it!!!
void TDIItem::paintEvent(QPaintEvent *)
{
    QStylePainter p(this);
    p.save();

    QStyleOptionButton option;

    option.features |= QStyleOptionButton::CommandLinkButton;
    option.text = QString();
    option.icon = QIcon();

    QSize pixmapSize(PLUGIN_ICON_SIZE, PLUGIN_ICON_SIZE);

    int vOffset = 0;
    int hOffset = 0;

//    Debug::addInfo(this->objectName(), "geometry (Pos, Size):", this->geometry(), sender());

    //Draw icon...
    p.drawControl(QStyle::CE_PushButton, option);
    if (!icon().isNull())
        p.drawPixmap(hOffset, vOffset, icon().pixmap(pixmapSize, QIcon::Normal, QIcon::On));
    //Draw text...
    int textflags = Qt::TextSingleLine;
    p.drawItemText(QWidget::contentsRect().translated(hOffset + pixmapSize.width(), vOffset + pixmapSize.height()/4)
                   ,textflags, option.palette, true, ItemText(), QPalette::ButtonText);
    p.restore();
}



void TDIItem::CreateLayoutOnFirstChild(TDIItem *item, QWidget *childWidget)
{
    if (item->layout()) {
        item->m_vboxLayout = qobject_cast<QVBoxLayout*>(item->layout());
    }
    else {
        QDesignerFormWindowInterface *frmWindow = QDesignerFormWindowInterface::findFormWindow(item);
        if (frmWindow) {
            item->m_vboxLayout = qobject_cast<QVBoxLayout*>(frmWindow->core()->widgetFactory()->createLayout(item, 0, qdesigner_internal::LayoutInfo::VBox));
        }
        if (!item->m_vboxLayout.isNull()) item->m_vboxLayout->addWidget(childWidget);
    }
    if (!item->m_vboxLayout.isNull()) {
        item->m_vboxLayout->setContentsMargins(PLUGIN_ICON_SIZE, PLUGIN_ICON_SIZE, PLUGIN_ICON_SIZE, 0);
        item->m_vboxLayout->setSpacing(0);
    }
}

void TDIItem::childEvent(QChildEvent *c_event)
{
    Debug::addInfo("childEvent: ", c_event->type(), c_event->child(), "parent_and_sender:", this, sender());
    if (c_event->type() == QEvent::Type::ChildAdded) {
    }
    else if (c_event->type() == QEvent::Type::ChildPolished) {
        if (c_event->child()->isWidgetType()) {
            QWidget* w = (QWidget*)c_event->child();
            if (qobject_cast<DeviceRoot*>(w) || !qobject_cast<TDIItem*>(w)) {
                if (!m_vboxLayout.isNull()) m_vboxLayout->removeWidget(w);
                w->setVisible(false);
                w->setParent(0);
            }
            else {
                if (m_vboxLayout.isNull()) CreateLayoutOnFirstChild(this, w);
            }
        }
        CalcTotalChildren(this);
    }
    else if (c_event->type() == QEvent::Type::ChildRemoved) {
        CalcTotalChildren(this);
    }

    //    QDesignerFormWindowInterface *formWindow = QDesignerFormWindowInterface::findFormWindow(this);
    //    QDesignerFormEditorInterface* formEditor = formWindow->core();
    //    QDesignerFormWindowManagerInterface* manager = formEditor->formWindowManager();
}

int TDIItem::CalcChildren(TDIItem* item)
{
    int count = 0;
    foreach (auto child, item->children()) {
        if (TDIItem* i = qobject_cast<TDIItem*>(child)) {
            count += 1 + CalcChildren(i);
        }
    }
    item->childItems = count;
    const QSize& sz = item->sizeHint();
    item->setMinimumSize(sz);
    return count;
}

void TDIItem::CalcTotalChildren(QWidget *widget)
{
    QDesignerFormWindowInterface *formWindow = QDesignerFormWindowInterface::findFormWindow(widget);
    if (formWindow) {
        DeviceRoot* root = qobject_cast<DeviceRoot*>(formWindow->mainContainer());
        if (root) {
            int totals = CalcChildren(root);
            root->childItems = totals+1; //NOTE: +1 impotent for actionAdjustSize !!!

            Debug::addInfo("resizing(root)...: ", root, root->sizeHint(), totals);
            if (root->height() < root->totalHeight(root->childItems)) {
                if (formWindow->core()->formWindowManager()) {
                    if (formWindow->core()->formWindowManager()->actionAdjustSize()) {
                        //gluk: formWindow->core()->formWindowManager()->actionAdjustSize()->trigger();
                    }
                }
//gluk:                formWindow->core()->formWindowManager()->actionAdjustSize()->trigger();
            }
        }
    }
}
QObject *TDIItem::cargo() const
{
    return m_cargo;
}

void TDIItem::setCargo(QObject *cargo)
{
    m_cargo = cargo;
}

NameLengthOptions TDIItem::nameLength() const
{
    return m_nameLength;
}

void TDIItem::setNameLength(const NameLengthOptions &nameLength)
{
    m_nameLength = nameLength;
}

void TDIItem::slotPropertyChanged(TDIItem* item, const QString &propertyName, const QVariant &newValue)
{
    Debug::addInfo("slotPropertyChanged(): ", item, propertyName, newValue);
//    if (propertyName == QStringLiteral("ChildAddressType")) {
//        if (FolderItem* fold = qobject_cast<FolderItem*>(item)) {
//            bool isChildAddressVisible = fold->childAddressType() != ChildAddressTypes::Auto
//                    && fold->childAddressType() != ChildAddressTypes::FirstSibLink;
//            TDIItem::SetPropertyVisible(item, QStringLiteral("ChildAddress"), isChildAddressVisible);
//        }
//    }
}

void TDIItem::changeEvent(QEvent *e)
{
    Debug::addInfo("changeEvent: ", e->type(), this, this->parent(), sender());
}

void TDIItem::objectNameChangedSlot(const QString &objName)
{
    if (m_itemText.isEmpty()) {
        if (qobject_cast<DeviceRoot*>(this)) {
            m_itemText = "Device";
            if (objName == "Form") {
                blockSignals(true);
                setObjectName("root");
                blockSignals(false);
            }
        }
        else {
            m_itemText = objName;
        }
        update();
    }
    Debug::addInfo("objectNameChanged:", this, objName, this->parent(),"text: ",m_itemText);
}

