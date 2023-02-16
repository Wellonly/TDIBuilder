#ifndef DEVICEITEM_H
#define DEVICEITEM_H

#include "app_config.h" //config.h was before 20151120
#include "securitygroups.h"
#include "tdiclasses.h"
//#include "tdi.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QPointer>
#include <QIcon>
#include <QAction>

#ifndef PLUGIN_ICON_SIZE
#define PLUGIN_ICON_SIZE 20
#endif

#define SymbolLinkPrefixStr "symbol:" /*NOTE: define used in sizeof */

enum ChildAddressTypes {
    Auto          = 0,     /*0 ChildAddress = first child key */
    SymbolLink    = 1,     /*0 ChildAddress = Symbol value */
    FirstSibLink  = 2,     /*1 ChildAddress = linked to first sibling key */
    ItemLink      = 3,     /*1 ChildAddress = linked to Another folder key */
    DefinedData   = 4,     /*2 ChildAddress = user data from a header file */
    UserDataDec   = 5,     /*2 ChildAddress = user const data */
    UserDataHex   = 6,     /*2 ChildAddress = user const data */
    ChildAddressUndefined,
};

//using namespace TDI;
class DeviceRoot;
class SecurityGroups;
class QDesignerFormEditorInterface;

class TDIItem : public SecurityGroups
{
    Q_OBJECT

    Q_PROPERTY(QString ItemText READ ItemText WRITE setItemText)
    Q_PROPERTY(SmoRecord::NameLengthOption NameLength READ nameLength WRITE setNameLength RESET NameLengthOption_reset) //NOTE: use a scopied typename not a typedef (it hides properties from designer)


public:
    TDIItem(QWidget *parent = 0, const QIcon& icon = QIcon());
    TDIItem(const TDIItem& other);
    ~TDIItem();

    QIcon icon() const;
    void setIcon(const QIcon& icon);

    QString ItemText() const;
    void setItemText(const QString &itemText);

    static DeviceRoot* DeviceRootItem(QObject *item);
    static void ObjectIspector_update(QWidget* widget);
    static bool SetPropertyChanged(TDIItem* item, const QString& propertyName, bool value, bool isPropertySheetToBeUpdated = true);
    static bool SetPropertyVisible(TDIItem* item, const QString& propertyName, bool value, bool isPropertySheetToBeUpdated = true);
    static void SetPropertyValue(TDIItem* item, const QString& propertyName, const QVariant &value);
    static QString FullPath(TDIItem* item); //old:ItemFullPath
    static TDIItem* ParentItem(TDIItem* item);
    static ChildAddressTypes ChildAddressType(const QString& childAddress);

    static const char* SymbolLinkPrefix;
    static const char* AutoChildKeyWord;
    static const char* FirstSibLinkKeyWord;

    QDesignerFormEditorInterface* core();

    NameLengthOptions nameLength() const;
    void setNameLength(const NameLengthOptions &nameLength);

    QObject *cargo() const;
    void setCargo(QObject *cargo);

public slots:
    static void slotPropertyChanged(TDIItem *item, const QString &propertyName, const QVariant& newValue);

signals:

protected:
    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    bool event(QEvent *e) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

    void childEvent(QChildEvent *c_event);
    void changeEvent(QEvent *e);

private slots:
    void objectNameChangedSlot(const QString &objName);

private: //private functions...
    inline int totalHeight(int child_items) const { return (child_items+1) * PLUGIN_ICON_SIZE;} /* +1 */
    void NameLengthOption_reset();

    static void CreateLayoutOnFirstChild(TDIItem* item, QWidget* childWidget);
    static int  CalcChildren(TDIItem *item);
    static void CalcTotalChildren(QWidget* widget);

private:
    QPointer<QVBoxLayout> m_vboxLayout;
    QString m_itemText;
    QIcon m_icon;
    int childItems;
    NameLengthOptions m_nameLength;
    QDesignerFormEditorInterface* m_core;
    QObject* m_cargo;
};
Q_DECLARE_METATYPE(TDIItem)
//Q_DECLARE_OPAQUE_POINTER(TDIItem*)
Q_DECLARE_METATYPE(TDIItem*)


#endif //DEVICEITEM_H
