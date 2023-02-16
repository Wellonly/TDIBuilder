/*
 *
*/

#include "symboltreemap.h"
#include "zvvlib.h"
#include "elf_parser.h"
#include "treelib.h"
#include "smodatatype_calc.h"

#include <QVBoxLayout>
#include <QMessageBox>
#include <QDateTime>
#include <QLabel>
#include <devicedata.h>


#include <deviceroot.h>
#include <devicefolder.h>
#include <deviceproperty.h>

class DeviceRoot;

SymbolsTreeMap::SymbolsTreeMap(TDIItem *targetItem)
: QDialog()
,m_targetItem(targetItem)
,m_refreshButton(new QPushButton(QIcon(QLatin1String(":/icons/refresh.png")),""))
,m_checkExternalOnly(new QCheckBox(tr("Scope: external only")))
,m_checkSectionGroup(new QCheckBox(tr("section")))
,m_checkUnitGroup(new QCheckBox(tr("unit")))
,m_checkTypedefGroup(new QCheckBox(tr("typedef")))
,m_checkPropertiesFilter(new QCheckBox(tr("on properties")))
,m_checkDataFilter(new QCheckBox(tr("on data")))
,m_checkEnumFilter(new QCheckBox(tr("on enums")))
,errorIcon(QIcon(QStringLiteral(":/icons/error.png")))
,okIcon(QIcon(QStringLiteral(":/icons/ok.png")))
,folderIcon(QIcon(QStringLiteral(":/icons/folder.png")))
,dataIcon(QIcon(QStringLiteral(":/icons/data.png")))
,propertyIcon(QIcon(QStringLiteral(":/icons/property.png")))
,listIcon(QIcon(QStringLiteral(":/icons/list.png")))
,m_lineFilter(new QLineEdit)
,m_tree(new QTreeWidget)
{
    m_tree->setColumnCount(1);
    m_header_node = new QTreeWidgetItem(QTreeWidgetItem::UserType);
    m_tree->setHeaderItem(m_header_node);
    connect(m_tree, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(treeItemChangedSlot(QTreeWidgetItem*,int)));
    m_tree->blockSignals(true);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *groupsLayout = new QHBoxLayout;

    connect(m_refreshButton, SIGNAL(clicked()), this, SLOT(treeRefreshSlot()));
    groupsLayout->addWidget(m_refreshButton);

    m_checkExternalOnly->setCheckState(Qt::CheckState::Checked);
    connect(m_checkExternalOnly, SIGNAL(stateChanged(int)), this, SLOT(scopeChangedSlot(int)));
    m_checkExternalOnly->setToolTip(tr("scope: all symbols or external only"));
    groupsLayout->addWidget(m_checkExternalOnly);

    groupsLayout->addStretch();

    QLabel* labelGroupBy = new QLabel(tr("Group by:"));
    groupsLayout->addWidget(labelGroupBy);

    m_checkSectionGroup->setCheckState(Qt::CheckState::Checked);
    connect(m_checkSectionGroup, SIGNAL(stateChanged(int)), this, SLOT(reGroupSlot(int)));
    m_checkSectionGroup->setToolTip(tr("Group by section"));
    groupsLayout->addWidget(m_checkSectionGroup);

    connect(m_checkUnitGroup, SIGNAL(stateChanged(int)), this, SLOT(reGroupSlot(int)));
    m_checkUnitGroup->setToolTip(tr("Group by compile unit"));
    groupsLayout->addWidget(m_checkUnitGroup);

    connect(m_checkTypedefGroup, SIGNAL(stateChanged(int)), this, SLOT(reGroupSlot(int)));
    m_checkTypedefGroup->setToolTip(tr("Group by typedef"));
    groupsLayout->addWidget(m_checkTypedefGroup);

    QHBoxLayout *filtersLayout = new QHBoxLayout;
    QLabel* labelFilters = new QLabel(tr("Filters:"));
    filtersLayout->addWidget(labelFilters);

    connect(m_lineFilter, SIGNAL(textChanged(QString)), this, SLOT(lineFilterTextChangedSlot(QString)));
    m_lineFilter->setToolTip(tr("Filter item's mask"));
    filtersLayout->addWidget(m_lineFilter);

    m_checkPropertiesFilter->setCheckState(Qt::CheckState::Checked);
    connect(m_checkPropertiesFilter, SIGNAL(stateChanged(int)), this, SLOT(reFilterPropertiesSlot(int)));
    m_checkPropertiesFilter->setToolTip(tr("Filter on properties"));
    filtersLayout->addWidget(m_checkPropertiesFilter);

    m_checkDataFilter->setCheckState(Qt::CheckState::Checked);
    connect(m_checkDataFilter, SIGNAL(stateChanged(int)), this, SLOT(reFilterDataSlot(int)));
    m_checkDataFilter->setToolTip(tr("Filter on data"));
    filtersLayout->addWidget(m_checkDataFilter);

    connect(m_checkEnumFilter, SIGNAL(stateChanged(int)), this, SLOT(reFilterEnumSlot(int)));
    m_checkEnumFilter->setToolTip(tr("Filter on enumerators"));
    filtersLayout->addWidget(m_checkEnumFilter);

    mainLayout->addLayout(groupsLayout);
    mainLayout->addLayout(filtersLayout);
    mainLayout->addWidget(m_tree);

    setLayout(mainLayout);
}

SymbolsTreeMap::~SymbolsTreeMap()
{
//    disconnect(this, 0, 0, 0);
    m_refreshButton->deleteLater(); // delete m_refreshButton;
    m_checkSectionGroup->deleteLater(); // delete m_checkSectionGroup;
    m_checkUnitGroup->deleteLater(); // delete m_checkUnitGroup;
    m_checkTypedefGroup->deleteLater(); //delete m_checkTypedefGroup;
    delete m_header_node;
    m_lineFilter->deleteLater(); //delete m_lineFilter;
    m_checkPropertiesFilter->deleteLater(); //delete m_checkPropertiesFilter;
    m_checkDataFilter->deleteLater(); //delete m_checkDataFilter;
    m_checkEnumFilter->deleteLater(); //delete m_checkEnumFilter;
    m_tree->deleteLater(); // delete m_tree;
    FreeBfd();
}

QSize SymbolsTreeMap::sizeHint() const
{
    return QSize(444, 666);
}

int SymbolsTreeMap::TreeMapExec(TDIItem *item, SelectLinkMode mode)
{
    m_targetItem = item;
    m_targetLinkMode = mode;
    setWindowTitle(tr("Select link for: %1.%2").arg(TDIItem::FullPath(item)).arg(EnumKey<SelectLinkMode>(mode)));
    DeviceRoot* root = TDIItem::DeviceRootItem(item);
    m_fileNew.setFile(root->dataSource());
    if (m_fileNew.lastModified().toMSecsSinceEpoch() != m_fileLoaded.lastModified().toMSecsSinceEpoch()) {
        RebuildTree(&m_fileNew, item);
    }
    else {
        RebuildTree(NULL, item);
    }
    return exec();
}

void SymbolsTreeMap::treeRefreshSlot()
{
    RebuildTree(&m_fileNew);
}

void SymbolsTreeMap::scopeChangedSlot(int)
{
    RebuildTree(&m_fileNew);
}

void SymbolsTreeMap::reGroupSlot(int)
{
    RebuildTree();
}

void SymbolsTreeMap::lineFilterTextChangedSlot(QString)
{
    RebuildTree();
}

void SymbolsTreeMap::reFilterDataSlot(int newDataFilterState)
{
    if (!newDataFilterState
        && m_checkPropertiesFilter->checkState() != Qt::CheckState::Checked
        && m_checkEnumFilter->checkState() != Qt::CheckState::Checked)
    {
        m_checkPropertiesFilter->setCheckState(Qt::CheckState::Checked);
    }
    RebuildTree();
}

void SymbolsTreeMap::reFilterEnumSlot(int newEnumFilterState)
{
    if (!newEnumFilterState
        && m_checkDataFilter->checkState() != Qt::CheckState::Checked
        && m_checkPropertiesFilter->checkState() != Qt::CheckState::Checked)
    {
        m_checkDataFilter->setCheckState(Qt::CheckState::Checked);
        m_checkPropertiesFilter->setCheckState(Qt::CheckState::Checked);
    }
    RebuildTree();
}

void SymbolsTreeMap::reFilterPropertiesSlot(int newPropertyFilterState)
{
    if (!newPropertyFilterState
            && m_checkDataFilter->checkState() != Qt::CheckState::Checked
            && m_checkEnumFilter->checkState() != Qt::CheckState::Checked)
    {
        m_checkDataFilter->setCheckState(Qt::CheckState::Checked);
    }
    RebuildTree();
}

void SymbolsTreeMap::treeItemChangedSlot(QTreeWidgetItem *item, int)
{
    if (item->checkState(0) == Qt::CheckState::Checked) {
        QString value = item->text(0);
        if (m_targetLinkMode == SelectLinkMode::ChildAddress)
        { //add "symbol:" prefix...
            value.prepend(TDIItem::SymbolLinkPrefix);
        }
        else if (m_targetLinkMode == SelectLinkMode::Data)
        {
            bool isOk = false;
            int smoDataType = item->data(0, Qt::UserRole).toInt(&isOk);
            if (isOk)
            { //calc right smoDataType for the TDIItem...
                bool isSymFunction = false;
                if (smoDataType >= 0x100) { //>=0x100: a function flag!!!
                    smoDataType &= 0xFF;
                    isSymFunction = true;
                }
                bool isEnumerator = smoDataType == SmoDataType::Enumerator;
                if (qobject_cast<DataItem*>(m_targetItem))
                {
                    if (isEnumerator) smoDataType = item->parent()->data(0, Qt::UserRole).toInt(&isOk); // read enumeration sizeb!!!
                    if (isSymFunction) isOk = false;
                }
                else if (qobject_cast<PropertyItem*>(m_targetItem))
                {
                    if (!isSymFunction) isOk = false;
                }
                else
                { //for folder & device...
                    if (!(isSymFunction || isEnumerator)) isOk = false;
                }
                if (isOk) {
                    TDIItem::SetPropertyValue(m_targetItem, "DataType", smoDataType);
                    if (isEnumerator)
                    { //add to enumerator its enumeration name...
                        value.prepend(item->parent()->text(0) + "::");
                    }
//debug only:       QMessageBox::information(this,"Changed slot!!!", tr("item: %1 %2 %3 %4").arg(m_targetItem->ItemText()).arg(item->text(0)).arg(EnumKey<SelectLinkMode>(m_targetLinkMode)).arg(EnumKey<SmoRecord::SmoDataType>(smoDataType)));
                }
            }
            if (!isOk)
            {
                item->setCheckState(0, Qt::CheckState::Unchecked);
                return; //do not exit from the dialog!!!
            }
        }
        TDIItem::SetPropertyValue(m_targetItem, EnumKey<SelectLinkMode>(m_targetLinkMode), value);  //NOTE:may be a TDIItem cast have to be here???
        done(0); //exit from the dialog!!!
    }
//    QMessageBox::information(this,"Changed slot!!!", tr("item: %1 %2 %3").arg(item->text(0)).arg(EnumKey<SelectLinkMode>(m_targetLinkMode)).arg(item->checkState(0)));
}

QTreeWidgetItem * SymbolsTreeMap::UnitNode(QTreeWidgetItem* folderNode, QString unitName)
{
    QTreeWidgetItem* unitNode = unitGroupMap.value(unitName, NULL);
    if (!unitNode) {
        unitNode = newFolderNode(folderNode, unitName);
        unitGroupMap[unitName] = unitNode;
    }

    return unitNode;
}

QTreeWidgetItem * SymbolsTreeMap::TypenameNode(QTreeWidgetItem* folderNode, QString typedefName)
{
    QTreeWidgetItem* typedefNode = typedefGroupMap.value(typedefName, NULL);
    if (!typedefNode) {
        typedefNode = newFolderNode(folderNode, typedefName);
        typedefGroupMap[typedefName] = typedefNode;
    }

    return typedefNode;
}

void SymbolsTreeMap::RebuildTree(QFileInfo *sfile, TDIItem *targetItem)
{
    bool isSourceToBeLoad = sfile != NULL;
    bool isNewTarget = targetItem != NULL;
    bool isGroupBySection = m_checkSectionGroup->checkState() == Qt::CheckState::Checked;
    bool isGroupByUnit = m_checkUnitGroup->checkState() == Qt::CheckState::Checked;
    bool isGroupByTypedef = m_checkTypedefGroup->checkState() == Qt::CheckState::Checked;
    bool isToBeScan_newTarget;
    QString newTargetText;
    if (isNewTarget) {
        newTargetText = targetItem->property(EnumKey<SelectLinkMode>(m_targetLinkMode)).toString();
        if (m_targetLinkMode == SelectLinkMode::ChildAddress)
        { //remove "symbol:" prefix...
            int dp = newTargetText.indexOf(TDIItem::SymbolLinkPrefix);
            if (dp == 0) newTargetText = newTargetText.mid(sizeof (SymbolLinkPrefixStr)-1);
        }
        if (!m_lineFilter->text().isEmpty()) {
            m_lineFilter->blockSignals(true);
            m_lineFilter->clear();
            m_lineFilter->blockSignals(false);
        }
        m_checkDataFilter->blockSignals(true);
        m_checkPropertiesFilter->blockSignals(true);
        m_checkEnumFilter->blockSignals(true);
        if (qobject_cast<DataItem*>(targetItem)) {
            if (newTargetText.indexOf("::") > 0)
            { //on enums delimeter...
                m_checkDataFilter->setCheckState(Qt::CheckState::Unchecked);
                m_checkEnumFilter->setCheckState(Qt::CheckState::Checked);
            }
            else {
                m_checkDataFilter->setCheckState(Qt::CheckState::Checked);
                m_checkEnumFilter->setCheckState(Qt::CheckState::Unchecked);
            }
            m_checkPropertiesFilter->setCheckState(Qt::CheckState::Unchecked);
        }
        else {
            m_checkDataFilter->setCheckState(Qt::CheckState::Unchecked);
            m_checkEnumFilter->setCheckState(Qt::CheckState::Unchecked);
            m_checkPropertiesFilter->setCheckState(Qt::CheckState::Checked);
        }
        m_checkDataFilter->blockSignals(false);
        m_checkPropertiesFilter->blockSignals(false);
        m_checkEnumFilter->blockSignals(false);
        isToBeScan_newTarget = !newTargetText.isEmpty();
    }
    else {
        isToBeScan_newTarget = false;
    }
    QString lineFiltr = m_lineFilter->text().trimmed();
    bool isLineFiltrEmpty = lineFiltr.isEmpty();
    bool isPropertiesToBeInList = m_checkPropertiesFilter->checkState() == Qt::CheckState::Checked;
    bool isDataToBeInList = m_checkDataFilter->checkState() == Qt::CheckState::Checked;
    bool isEnumDataToBeInList = m_checkEnumFilter->checkState() == Qt::CheckState::Checked;
    bool isDataOrPropertyToBeOutfiltered = !isPropertiesToBeInList || !isDataToBeInList;
    m_tree->blockSignals(true);
    m_tree->clear();
    if (isSourceToBeLoad) {
        m_fileLoaded.setFile(sfile->filePath());
        m_header_node->setText(0,tr("Source: %1 (%2)").arg(m_fileLoaded.fileName()).arg(m_fileLoaded.lastModified().toString()));
        m_refreshButton->setToolTip(tr("Refresh: %1").arg(m_fileLoaded.filePath()));
        m_sourceLoadResultCode = elf_parser(m_fileLoaded.absoluteFilePath().toLocal8Bit().data(), m_checkExternalOnly->checkState() == Qt::Checked);
    }
    if (m_sourceLoadResultCode == msg_Success)
    { //fill elfSymbolsMap....
        QTreeWidgetItem* foundedTargetNode = NULL;
        QTreeWidgetItem* folderNode;
        QString sectionName;
        QString unitName;
        QString typeName;

        sectionGroupMap.clear();
        unitGroupMap.clear();
        typedefGroupMap.clear();
        elfSymbolsMap.clear();
        enumsMap.clear();

        if (isPropertiesToBeInList || isDataToBeInList)
        { //first: do symbols tree...
            auto cou = SymsCount();
            void** symstab = SymsTable();
            for (auto i = 0; i < cou; i++)
            {
                void* sym = *symstab++;
                bool isSymAfunction = SymIsFunction(sym);
                if (isDataOrPropertyToBeOutfiltered)
                { //if activated, then do filtering...
                    if (isSymAfunction) {
                        if (!isPropertiesToBeInList) {
                            continue;
                        }
                    }
                    else {
                        if (!isDataToBeInList) {
                            continue;
                        }
                    }
                }
                QString key = QString(SymName(sym));
                if (!isLineFiltrEmpty && key.indexOf(lineFiltr) == -1) {
                    continue;
                }
    //we here only if the sym doesn't outfiltered!!!
                elfSymbolsMap.insert(key, sym);
                sectionName = QString(SymSectionName(sym));
                unitName = QString(SymSourceFileName(sym));
                typeName = QString(SymbolTypeName(sym));
                if (isGroupBySection) {
                    QTreeWidgetItem* sectionNode = sectionGroupMap.value(sectionName, NULL);
                    if (!sectionNode) {
                        sectionNode = newFolderNode(m_tree->invisibleRootItem(), sectionName);
                        sectionGroupMap[sectionName] = sectionNode;
                    }
                    folderNode = sectionNode;
                }
                else {
                    folderNode = m_tree->invisibleRootItem();
                }
                if (isGroupByUnit) {
                    folderNode = UnitNode(folderNode, unitName);
                }
                if (isGroupByTypedef) {
                    folderNode = TypenameNode(folderNode, typeName);
                }

                QTreeWidgetItem* node = new QTreeWidgetItem(folderNode, QTreeWidgetItem::UserType);
                node->setFlags(node->flags() | Qt::ItemIsTristate);
                node->setCheckState(0, Qt::Unchecked);
                node->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
                node->setIcon(0, iif(isSymAfunction, propertyIcon, dataIcon));
                node->setText(0, key);
                int dataSizeb;
                int encod;
                int smoDT;
                if (isSymAfunction) {
                    encod = SymbolParameter_encoding(sym,0);
                    dataSizeb = SymbolParameter_BasetypePointerSize(sym,0);
                    if (dataSizeb == 0) dataSizeb = SymbolParameter_basetype_dataSize(sym,0);
                    QString SymbolParam_typename(SymbolParameter_typeName(sym,0));
                    smoDT = TdiLib::DataType_onTypename(SymbolParam_typename);
                    if (smoDT == SmoRecord::SmoDataType::Undefined) smoDT = TdiDataType_calc(dataSizeb, encod);
                    node->setToolTip(0,
                     tr("Function name: %1\n Section: %2\n "
                        "Absolute address: 0x%3\n "
                        "Size(bytes): %4\n "
                        "Return: type name: %5; size(bytes): %6\n "
                        "Parameter/data: type name: %7; size(bytes): %8; SmoDataType::%9; encoded: 0x%10\n "
                        "File: %11")
                     .arg(key)
                     .arg(sectionName)
                     .arg(SymAbsoluteValue(sym),0,16)
                     .arg(SymBodySize(sym))
                     .arg(typeName)
                     .arg(SymbolDataSize(sym))
                     .arg(SymbolParam_typename)
                     .arg(dataSizeb)
                     .arg(EnumKey<SmoRecord::SmoDataType>(smoDT))
                     .arg(encod,0,16)
                     .arg(unitName));
                }
                else
                { // if it's not a function (isSymAfunction == false)...
                    encod = SymbolEncoding(sym);
                    dataSizeb = SymBodySize(sym);
                    smoDT = TdiDataType_calc(dataSizeb, encod);
                    node->setToolTip(0,
                     tr("Data object name: %1\n "
                        "Section: %2\n "
                        "Absolute address: 0x%3\n "
                        "Data: type name: %4; size(bytes): %5; SmoDataType::%6; encoded: 0x%7\n "
                        "File: %8")
                     .arg(key)
                     .arg(sectionName)
                     .arg(SymAbsoluteValue(sym),0,16)
                     .arg(typeName)
                     .arg(dataSizeb)
                     .arg(EnumKey<SmoRecord::SmoDataType>(smoDT))
                     .arg(encod,0,16)
                     .arg(unitName));
                }
                node->setData(0, Qt::UserRole, QVariant::fromValue(iif(isSymAfunction, smoDT | 0x100, smoDT)));
                if (isToBeScan_newTarget && strcmp(key.toLocal8Bit().constData(), newTargetText.toLocal8Bit().constData()) == 0) {
                    foundedTargetNode = node;
                }
            } //for loop
        }
        if (isEnumDataToBeInList)
        { //fills enumerators data...
            QStringList newEnumText;
            bool isEnumerationFound = false;
            if (isToBeScan_newTarget) {
                newEnumText = newTargetText.split("::");
                if (isSourceToBeLoad && !(isPropertiesToBeInList || isDataToBeInList))
                { //for this circumstances we need to refresh the file symbols...
                    auto cou = SymsCount();
                    void** symstab = SymsTable();
                    for (auto i = 0; i < cou; i++)
                    {
                        SymDwarf2ExtraInfo(*symstab++); //read up extrainfo!!!
                    }
                }
            }
            QTreeWidgetItem* sectionNode = iif(isGroupBySection, newFolderNode(m_tree->invisibleRootItem(), QStringLiteral("Enumerators")), NULL);
            auto cou = Enumerations_count();
            void** enums_table = Enumeration_table();
            for (auto i = 0; i < cou; i++)
            {
                void* etion = *enums_table++;
                QString ename = QString(Enumeration_name(etion));
                if (!isLineFiltrEmpty && ename.indexOf(lineFiltr) == -1) {
                    continue;
                }
                enumsMap.insert(ename, etion);
                unitName = QString(Enumeration_sourceFileName(etion));
                int enum_sizeb = Enumeration_sizeb(etion);
                typeName = EnumKey<SmoRecord::SmoDataType>(enum_sizeb);
                folderNode = iif(isGroupBySection, sectionNode, m_tree->invisibleRootItem());
                if (isGroupByUnit) {
                    folderNode = UnitNode(folderNode, unitName);
                }
                if (isGroupByTypedef) {
                    folderNode = TypenameNode(folderNode, typeName);
                }

                QTreeWidgetItem* enumeration_node = new QTreeWidgetItem(folderNode, QTreeWidgetItem::UserType);
                enumeration_node->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
                enumeration_node->setIcon(0, listIcon);
                enumeration_node->setText(0, ename);
                enumeration_node->setData(0, Qt::UserRole, QVariant::fromValue(enum_sizeb)); //SmoDataType::Enumerator

                enumeration_node->setToolTip(0,
                 tr("Enumeration name: %1\n"
                    "  Data: type name: %2; size(bytes): %3\n"
                    "  File: %4")
                 .arg(ename)
                 .arg(typeName)
                 .arg(enum_sizeb)
                 .arg(unitName));

                if (isToBeScan_newTarget) {
                    isEnumerationFound = QString::compare(ename, newEnumText[0]) == 0;
                }
                void* emem = Enumerator_first(etion);
                while (emem) {
                    QString emem_name = QString(Enumerator_name(emem));
                    long emem_value = Enumerator_value(emem);

                    QTreeWidgetItem* emem_node = new QTreeWidgetItem(enumeration_node, QTreeWidgetItem::UserType);
                    emem_node->setFlags(emem_node->flags() | Qt::ItemIsTristate);
                    emem_node->setCheckState(0, Qt::Unchecked);
                    emem_node->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
                    emem_node->setIcon(0, dataIcon);
                    emem_node->setText(0, emem_name);
                    emem_node->setData(0, Qt::UserRole, QVariant::fromValue(SmoDataType::Enumerator));
                    emem_node->setToolTip(0, QString("%1::%2 = %3").arg(ename).arg(emem_name).arg(emem_value));

                    if (isEnumerationFound && QString::compare(emem_name, newEnumText[1]) == 0) {
                        foundedTargetNode = emem_node;
                    }
                    emem = Enumerator_next(emem);
                } //end enumerators loop
            } //end enumerations loop
        }
        if (foundedTargetNode) {
            for (QTreeWidgetItem* n = foundedTargetNode->parent(); n ; n = n->parent()) {
                n->setExpanded(true);
            }
            TreeLib::setItemVisibleSelectedChecked(foundedTargetNode);
        }
        else {
            if (m_tree->topLevelItemCount() == 1) {
                m_tree->topLevelItem(0)->setExpanded(true);
            }
        }
        m_header_node->setIcon(0, okIcon);
    }
    else {
        m_header_node->setIcon(0, errorIcon);
    }

    m_tree->blockSignals(false);
//    QMessageBox::information(this,"SymbolsTreeMap rebuilded!!!", tr("build params: %1 %2 %3").arg(isGroupByUnit).arg(isGroupByTypedef).arg(m_fileLoaded.fileName()));
}

QTreeWidgetItem *SymbolsTreeMap::newFolderNode(QTreeWidgetItem *parent, QString text)
{
    QTreeWidgetItem* node = new QTreeWidgetItem(parent, QTreeWidgetItem::UserType);
    node->setIcon(0, folderIcon);
    node->setText(0, text);
    return node;
}

