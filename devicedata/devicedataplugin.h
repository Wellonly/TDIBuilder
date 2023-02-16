#ifndef DEVICEDATAPLUGIN_H
#define DEVICEDATAPLUGIN_H

#include <QtUiPlugin/QDesignerCustomWidgetInterface> /* old: #include <QDesignerCustomWidgetInterface> */


class DeviceDataPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)


public:
    DeviceDataPlugin(QObject *parent = 0);

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

signals:

public slots:

private:
    bool m_initialized;
};

#endif
