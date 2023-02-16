#ifndef DEBUGINFOPLUGIN_H
#define DEBUGINFOPLUGIN_H

#include "debuginfo.h"

#include <QtUiPlugin/QDesignerCustomWidgetInterface> //#include <QDesignerCustomWidgetInterface>
#include <QDesignerFormEditorInterface>
#include <QPointer>

class DebugInfoPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetInterface")
#endif // QT_VERSION >= 0x050000

public:
    DebugInfoPlugin(QObject *parent = 0);

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
    void initialize(QDesignerFormEditorInterface *formEditor);

private:
    bool m_initialized;
    QPointer<DebugInfo> m_debugInfo;
    QPointer<DebugInfo> m_debugPtr;
    QPointer<LogWidget> m_LogPtr;

    QPointer<QDesignerFormEditorInterface> m_formEditor;
};

#endif
