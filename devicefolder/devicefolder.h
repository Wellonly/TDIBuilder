#ifndef DEVICEFOLDER_H
#define DEVICEFOLDER_H

#include "deviceitem.h"

class FolderItem : public TDIItem
{
    Q_OBJECT
    Q_PROPERTY(FolderTypes FolderType READ folderType WRITE setFolderType)
    Q_PROPERTY(QString ChildAddress READ childAddress WRITE setChildAddress RESET childAddressReset)

public:
    FolderItem(QWidget *parent = 0);
    FolderItem(const FolderItem& other);
    ~FolderItem();

#include "smofoldertype.h"
    Q_ENUM(FolderTypes) //do not add ";"

    QString childAddress() const;
    void setChildAddress(const QString &childAddress);

    FolderTypes folderType() const;
    void setFolderType(const FolderTypes &folderType);

protected:

private slots:
    void childAddressReset();

private:
    FolderTypes m_folderType;
    QString m_childAddress;

};
Q_DECLARE_METATYPE(FolderItem) //do not add ";"
Q_DECLARE_METATYPE(FolderItem*) //do not add ";"
typedef FolderItem::FolderTypes FolderTypes;

#endif
