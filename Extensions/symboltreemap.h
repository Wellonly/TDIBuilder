#ifndef SymbolsTREEMAP_H
#define SymbolsTREEMAP_H

#include "GlobalStatusErrorCodes.h"

#include "deviceitem.h"
#include <QDialog>
#include <QTreeWidget>
#include <QWidget>
#include <QCheckBox>
#include <QPushButton>
#include <QFileInfo>
#include <QIcon>
#include <QLineEdit>

using namespace std;

typedef QMap<QString,void*> Pointers_map_t;
typedef QMap<QString,QTreeWidgetItem*> Groups_map_t;

class SymbolsTreeMap : QDialog
{
    Q_OBJECT
public:
    SymbolsTreeMap(TDIItem *targetItem = 0);
    ~SymbolsTreeMap();

//NOTE: SelectLinkMode item's names the same with TDIItem property names!!! (see code: SetPropertyValue())
    enum SelectLinkMode { Data,/*Parameter,*/ ChildAddress};
    Q_ENUM(SelectLinkMode)

    QSize sizeHint() const Q_DECL_OVERRIDE;

    int TreeMapExec(TDIItem *item, SelectLinkMode mode);

private slots:
    void treeRefreshSlot(void);
    void scopeChangedSlot(int);
    void reGroupSlot(int);
    void lineFilterTextChangedSlot(QString);
    void reFilterDataSlot(int newDataFilterState);
    void reFilterEnumSlot(int newEnumFilterState);
    void reFilterPropertiesSlot(int newPropertyFilterState);
    void treeItemChangedSlot(QTreeWidgetItem* item, int);

private:
    void RebuildTree(QFileInfo* sfile=NULL, TDIItem* targetItem=NULL);
    QTreeWidgetItem* newFolderNode(QTreeWidgetItem *parent, QString text);
    QTreeWidgetItem* UnitNode(QTreeWidgetItem* folderNode, QString unitName);
    QTreeWidgetItem* TypenameNode(QTreeWidgetItem* folderNode, QString typedefName);

private:
    G8 m_sourceLoadResultCode;
    TDIItem* m_targetItem;
    SelectLinkMode m_targetLinkMode;
    QPushButton *m_refreshButton;
    QCheckBox* m_checkExternalOnly;
    QCheckBox* m_checkSectionGroup;
    QCheckBox* m_checkUnitGroup;
    QCheckBox* m_checkTypedefGroup;
    QCheckBox* m_checkPropertiesFilter;
    QCheckBox* m_checkDataFilter;
    QCheckBox* m_checkEnumFilter;
    QString m_sourceFName;
    QFileInfo m_fileLoaded;
    QFileInfo m_fileNew;

    Pointers_map_t elfSymbolsMap;
    Pointers_map_t enumsMap;
    Groups_map_t sectionGroupMap;
    Groups_map_t unitGroupMap;
    Groups_map_t typedefGroupMap;

    QTreeWidgetItem* m_header_node;
    QIcon errorIcon;
    QIcon okIcon;
    QIcon folderIcon;
    QIcon dataIcon;
    QIcon propertyIcon;
    QIcon listIcon;
    QLineEdit* m_lineFilter;
    QTreeWidget* m_tree;
};

#endif // SymbolsTREEMAP_H
