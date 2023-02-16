
#include "tdimap.h"
#include "treelib.h"
#include "deviceroot.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
//#include <QMessageBox> //QMessageBox() for debug only

static TDIMapItemInfo* rootItemInfo = NULL;

TDIMap::TDIMap(TDIItem *targetItem)
: QDialog()
,m_targetItem(targetItem)
,m_checkAuto(new QCheckBox(TDIItem::AutoChildKeyWord))
,m_checkFirstSibLink(new QCheckBox(TDIItem::FirstSibLinkKeyWord))
,m_header_node(new QTreeWidgetItem(QTreeWidgetItem::UserType))
,m_tree(new QTreeWidget)
{
    m_tree->setColumnCount(1);
    m_header_node->setText(0, "TDI items");
    m_tree->setHeaderItem(m_header_node);

    connect(m_tree, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(treeItemChangedSlot(QTreeWidgetItem*,int)));

    connect(m_checkAuto, SIGNAL(stateChanged(int)), this, SLOT(checkStateChangedSlot(int)));
    connect(m_checkFirstSibLink, SIGNAL(stateChanged(int)), this, SLOT(checkStateChangedSlot(int)));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *groupsLayout = new QHBoxLayout;
    groupsLayout->addWidget(m_checkAuto);
    groupsLayout->addWidget(m_checkFirstSibLink);
    groupsLayout->addStretch();

    mainLayout->addLayout(groupsLayout);
    mainLayout->addWidget(m_tree);

    setLayout(mainLayout);
}

TDIMap::~TDIMap()
{
//    disconnect(this, 0, 0, 0);
//    disconnect(m_tree, 0, 0, 0);
//    disconnect(m_checkAuto, 0, 0, 0);
//    disconnect(m_checkFirstSibLink, 0, 0, 0);

    m_checkAuto->deleteLater(); // delete m_checkAuto;
    m_checkFirstSibLink->deleteLater(); //delete m_checkFirstSibLink;
    m_tree->deleteLater(); //delete m_tree;

    if (rootItemInfo) rootItemInfo->deleteLater(); //delete rootItemInfo;
    rootItemInfo = NULL;
}

QSize TDIMap::sizeHint() const
{
    return QSize(444, 666);
}

static TDIItem *staticTarget;
static QTreeWidgetItem *staticFoundedItem;
static QTreeWidget* staticMaptree;
int TDIMap::TDIMapExec(TDIItem *item)
{
    SignalsToBeBlocked(true);
    staticTarget = m_targetItem = item;
    setWindowTitle(tr("Select item for: %1.ChildAddress").arg(TDIItem::FullPath(item)));

    staticMaptree = m_tree;
    m_tree->clear();
    delete rootItemInfo;
    rootItemInfo = new TDIMapItemInfo();
    m_tree->addTopLevelItem(rootItemInfo->mapitem());
    rootItemInfo->mapitem()->setExpanded(true);

    FolderItem* fold = qobject_cast<FolderItem*>(item);
    bool isAutoToBeChecked = false;
    bool isFirstSiblingToBeChecked = false;
    staticFoundedItem = NULL;
    if (fold->childAddress().indexOf('/') == -1) {
        auto itr = [](TDIItem* itm)->void {
                new TDIMapItemInfo(itm);
        };
        TreeLib::CalcQObjectNodes<TDIItem>(qobject_cast<QObject*>(rootItemInfo->tdiItem()), itr, false);
        if (!fold->childAddress().isEmpty()) {
            isAutoToBeChecked = !QString::compare(m_checkAuto->text(), fold->childAddress());
            isFirstSiblingToBeChecked = !isAutoToBeChecked
                    && !QString::compare(m_checkFirstSibLink->text(), fold->childAddress());
        }
    }
    else {
        auto itr = [fold](TDIItem* itm)->void {
            TDIMapItemInfo* it = new TDIMapItemInfo(itm);
            if (!QString::compare(TDIItem::FullPath(itm), fold->childAddress())) {
                staticFoundedItem = it->mapitem();
            }
        };
        TreeLib::CalcQObjectNodes<TDIItem>(qobject_cast<QObject*>(rootItemInfo->tdiItem()), itr, false);
        if (staticFoundedItem) TreeLib::setItemVisibleSelectedChecked(staticFoundedItem);
    }
    m_checkAuto->setChecked(isAutoToBeChecked);
    m_checkFirstSibLink->setChecked(isFirstSiblingToBeChecked);
    SignalsToBeBlocked(false);
    return exec();
}

void TDIMap::treeItemChangedSlot(QTreeWidgetItem *item, int)
{
    if (item->checkState(0) == Qt::CheckState::Checked) {
        setItemValue(TreeLib::ItemFullPath(item));
        done(0);
    }
}

void TDIMap::checkStateChangedSlot(int newState)
{
    if (newState == Qt::CheckState::Checked) {
        QCheckBox* check = qobject_cast<QCheckBox*>(sender());
        if (check) setItemValue(check->text());
        done(0);
    }
}

void TDIMap::SignalsToBeBlocked(bool isToBeBlocked)
{
    m_tree->blockSignals(isToBeBlocked);
    m_checkAuto->blockSignals(isToBeBlocked);
    m_checkFirstSibLink->blockSignals(isToBeBlocked);
}

void TDIMap::setItemValue(QString itemValue)
{
    TDIItem::SetPropertyValue(m_targetItem, QStringLiteral("FolderType"), FolderTypes::LinkToFolder);
    TDIItem::SetPropertyValue(m_targetItem, QStringLiteral("ChildAddress"), itemValue);
}

TDIMapItemInfo::TDIMapItemInfo()
{
    m_tdiItem = qobject_cast<TDIItem*>(TDIItem::DeviceRootItem(staticTarget));
    m_mapitem = TreeLib::newItem(m_tdiItem->ItemText(), m_tdiItem->icon());
    m_mapitem->setFlags(m_mapitem->flags() | Qt::ItemIsUserCheckable); //ItemIsTristate
    m_mapitem->setCheckState(0, Qt::Unchecked);
    m_tdiItem->setCargo(this);
}

TDIMapItemInfo::TDIMapItemInfo(TDIItem *item)
{
    m_tdiItem = item;
    TDIItem* parentItem = TDIItem::ParentItem(item);
    TDIMapItemInfo* parentInfo = iif(parentItem, qobject_cast<TDIMapItemInfo*>(parentItem->cargo()), NULL);
    m_mapitem = new QTreeWidgetItem(QTreeWidgetItem::UserType);
    m_mapitem->setFlags(m_mapitem->flags() | Qt::ItemIsUserCheckable); //ItemIsTristate
    m_mapitem->setCheckState(0, Qt::Unchecked);
    m_mapitem->setIcon(0, item->icon());
    if (parentInfo) {
        m_mapitem->setText(0, item->ItemText());
        parentInfo->m_mapitem->addChild(m_mapitem);
    }
    else {
        m_mapitem->setText(0, item->ItemText().prepend("assertion failed:"));
        staticMaptree->addTopLevelItem(m_mapitem);
    }
    m_mapitem->setExpanded(true);
    item->setCargo(this);
    setParent(parentInfo);
}
QTreeWidgetItem *TDIMapItemInfo::mapitem() const
{
    return m_mapitem;
}
TDIItem *TDIMapItemInfo::tdiItem() const
{
    return m_tdiItem;
}



