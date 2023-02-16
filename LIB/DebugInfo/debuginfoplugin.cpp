#include "debuginfoplugin.h"

#include <QExtensionManager>
#include <QtPlugin>

DebugInfoPlugin::DebugInfoPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void DebugInfoPlugin::initialize(QDesignerFormEditorInterface* formEditor)
{
    if (m_initialized) return;
    m_formEditor = formEditor;
    m_debugInfo = new DebugInfo(formEditor);

//    m_debugPtr = Debug::GetPointer<DebugInfo>(formEditor->topLevel(), "DebugInformer");
//    Debug::addInfo<DebugInfo>(m_debugPtr,QVariant::fromValue(QStringLiteral("DebugInfoPlugin found and initialized")));

//    m_LogPtr = Debug::GetPointer<LogWidget>(formEditor->topLevel(), "LogWidget");
//    Debug::addInfo<LogWidget>(m_LogPtr,QVariant::fromValue(QStringLiteral("LogWidget found and initialized")));

    m_initialized = true;
}

bool DebugInfoPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *DebugInfoPlugin::createWidget(QWidget* /*parent*/)
{
    if (m_debugInfo) {
        m_debugInfo->setVisible(true);
    }
    else
    {
        m_debugInfo = new DebugInfo(m_formEditor.data());
        m_debugInfo->addInfo("DebugInfoPlugin: reinitialized!!!");
    }
    return 0;
}

QString DebugInfoPlugin::name() const
{
    return QLatin1String("DebugInfo");
}

QString DebugInfoPlugin::group() const
{
    return QLatin1String("Debug");
}

QIcon DebugInfoPlugin::icon() const
{
    return QIcon(QLatin1String(":/Well.png"));
}

QString DebugInfoPlugin::toolTip() const
{
    return QLatin1String("Debug informer");
}

QString DebugInfoPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool DebugInfoPlugin::isContainer() const
{
    return true;
}

QString DebugInfoPlugin::domXml() const
{
    return QLatin1String("<widget class=\"DebugInfo\" name=\"debugInfo\">\n</widget>\n");
}

QString DebugInfoPlugin::includeFile() const
{
    return QLatin1String("debuginfo.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(debuginfoplugin, DebugInfoPlugin)
#endif // QT_VERSION < 0x050000
