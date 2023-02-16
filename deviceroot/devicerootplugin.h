#ifndef DEVICEROOTPLUGIN_H
#define DEVICEROOTPLUGIN_H

#include <QtUiPlugin/QDesignerCustomWidgetInterface> /* old: #include <QDesignerCustomWidgetInterface> */

class DeviceRootPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    DeviceRootPlugin(QObject *parent = 0);

    bool isContainer() const;
    bool isInitialized() const;
    QIcon icon() const;
    QString domXml() const;
    QString group() const;
    QString includeFile() const;
    QString name() const;
    QString toolTip() const;
    QString whatsThis() const;
    QWidget *createWidget(QWidget *parent);
    void initialize(QDesignerFormEditorInterface *core);

public slots:
    void slotCheckProperty(const QString& propertyName, const QVariant& newValue);

private:
    bool m_initialized;
    bool m_connected;
    QDesignerFormEditorInterface* m_core;
};
//Q_DECLARE_METATYPE(DeviceRootPlugin)

#endif
