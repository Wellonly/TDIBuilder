#ifndef TDIMAP_H
#define TDIMAP_H

#include "devicefolder.h"
#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QIcon>
#include <QCheckBox>
#include <QTreeWidget>

class TDIMap : public QDialog
{
    Q_OBJECT
public:
    TDIMap(TDIItem *targetItem = 0);
    ~TDIMap();

    QSize sizeHint() const Q_DECL_OVERRIDE;

    int TDIMapExec(TDIItem *item);

private slots:
    void treeItemChangedSlot(QTreeWidgetItem* item, int);
    void setItemValue(QString itemValue);
    void checkStateChangedSlot(int newState);

private:
    void SignalsToBeBlocked(bool isToBeBlocked);

private:
    TDIItem* m_targetItem;
    QCheckBox* m_checkAuto;
    QCheckBox* m_checkFirstSibLink;
    QTreeWidgetItem* m_header_node;
    QTreeWidget* m_tree;
};

class TDIMapItemInfo : public QObject
{
    Q_OBJECT
public:
    TDIMapItemInfo();
    TDIMapItemInfo(TDIItem* item);
    virtual ~TDIMapItemInfo() {}

    QTreeWidgetItem *mapitem() const;

    TDIItem *tdiItem() const;

private:
    TDIItem* m_tdiItem;
    QTreeWidgetItem* m_mapitem;
};

#endif // TDIMAP_H
