#ifndef TDIEXTENSIONFACTORY_H
#define TDIEXTENSIONFACTORY_H

#include "deviceitem.h"
#include "symboltreemap.h"
#include "tdimap.h"

#include <QFileDialog>
#include <QObject>
#include <QExtensionFactory>
#include <QDesignerTaskMenuExtension>
#include <QDesignerFormEditorInterface>

class QAction;
class QExtensionManager;

/////////////////////////////////////////// ExtensionFactory...
class TDITaskMenuExtensionFactory : public QExtensionFactory
{
    Q_OBJECT
public:
    TDITaskMenuExtensionFactory(QDesignerFormEditorInterface* core);
    ~TDITaskMenuExtensionFactory();

    QDesignerFormEditorInterface *core() const;

    SymbolsTreeMap *symbolsTreeMap() const;
    void setSymbolsTreeMap(SymbolsTreeMap *treeMap);

    TDIMap *tdiMap() const;
    void setTdiMap(TDIMap *tdiMap);

protected:
    QObject *createExtension(QObject *object, const QString &iid, QObject *extensionsParent) const Q_DECL_OVERRIDE;

private:
    QDesignerFormEditorInterface* m_core;
    SymbolsTreeMap* m_symbolsTreeMap;
    TDIMap* m_tdiMap;
};

/////////////Menu...
class TDITaskMenu : public QObject, public QDesignerTaskMenuExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerTaskMenuExtension)

public:
    TDITaskMenu(QObject *factory, TDIItem *deviceItem, QObject *menuParent);

    QAction *preferredEditAction() const Q_DECL_OVERRIDE;
    QList<QAction*> taskActions()  const Q_DECL_OVERRIDE;

    TDIItem *deviceItem() const;

    TDITaskMenuExtensionFactory *factory() const;

private slots:
    static void SelectLinkToAddress(TDITaskMenu *menu);
//    static void SelectLinkToParameter(TDITaskMenu *menu);
    static void SelectSymbolLinkToChildAddress(TDITaskMenu *menu);
    static void SelectTDIItemToChildAddress(TDITaskMenu *menu);
    static void SelectLinkTo(TDITaskMenu *menu, SymbolsTreeMap::SelectLinkMode mode);
    static void SelectDeviceHeader(TDITaskMenu *menu);
    static void SelectDeviceDataSource(TDITaskMenu *menu);

private:
    QList<QAction*> menuActions;
    TDIItem *m_deviceItem;
    TDITaskMenuExtensionFactory* m_factory;
};

//template <class T>
typedef void (*menuActionMethodPtr)(TDITaskMenu*);
class TDITaskMenuAction : public QAction {
    Q_OBJECT
public:
    TDITaskMenuAction(const QString& text, TDITaskMenu* parent, menuActionMethodPtr method);
    ~TDITaskMenuAction();
private: //public slots:
    void Action();

private:
    TDITaskMenu* m_menu;
    menuActionMethodPtr m_method;
};

#endif // TDIEXTENSIONFACTORY_H
