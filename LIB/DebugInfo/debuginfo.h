/// Debug informer for Qt Designer
/// Copyright 2015; Author: Valentin Zotov
/// Support: wellmanmail@gmail.com
#ifndef DEBUGINFO_H
#define DEBUGINFO_H

#include "app_config.h" //before 20151216 was: "config.h"
#include "logwidget.h"

#include <QDebug>
#include <QString>
#include <QAction>
#include <QTextEdit>
#include <QDesignerFormEditorInterface>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QVariant>
#include <QMessageBox>
#include <QWidget>

#ifndef DEBUG_ACTION_NAME
#define DEBUG_ACTION_NAME "DebugInformer"
#endif

#ifndef DEBUG_IS_ADDINFO
#define DEBUG_IS_ADDINFO 1
#endif

class DebugInfo : public QDockWidget
{
    Q_OBJECT

public:
    DebugInfo(QDesignerFormEditorInterface* formEditor);
    DebugInfo(const DebugInfo& other);
    ~DebugInfo();
    void addInfo(QVariant usrData);

public slots:
    void debugActionTriggered(bool);
private:
    QDesignerFormEditorInterface* m_formEditor;
    QAction* addInfoAction;
    LogWidget* logWidget;
    QVBoxLayout* debLayout;
};
Q_DECLARE_METATYPE(DebugInfo*)

namespace Debug
{
/*
 * The Debug usage examples:
    Debug::MessageBox(QWidgetPtr, "usage before: Debug::Connect()");
    Debug::Connect(QWidgetPtr);
    Debug::MessageBox("after: Debug::Connect()");
    Debug::addInfo("Debug connected!!!", ...);
*/
    static QWidget* TopWidget = 0;
    static QAction* DebugerAction = 0;
    static QString  DebugStr;
    static QDebug   DebugObj(&DebugStr);

    inline void MessageBox(QWidget* widget, QString text) {
        QMessageBox::information(widget,
         (widget ? widget->windowTitle() : "-") + " DebugSpace.MessageBox", text);
    }

    inline void MessageBox(QString text) {
        Debug::MessageBox(Debug::TopWidget, text);
    }

    inline bool Connect(QWidget *topWidget) {
        Debug::TopWidget = topWidget;
        if (topWidget) {
            foreach (QAction* act, topWidget->actions()) {
                if (act->text() == DEBUG_ACTION_NAME) {
                    Debug::DebugerAction = act;
                    return true;
                }
            }
            //MessageBox("Debug has not connected!!!");
        }
        return false;
    }

    inline bool IsConnected()
    {
        return Debug::TopWidget && Debug::DebugerAction;
    }

    inline bool _AddInfo_inlineHelper(QString info)
    {
        if (IsConnected()) {
            Debug::DebugerAction->setData(info);
            Debug::DebugerAction->triggered();
            return true;
        }
        return false;
    }

    template<class T>
    void _AddInfo_ListHelper(T value)
    {
        DebugObj << value;
    }
    template<class T, class ...Rest>
    void _AddInfo_ListHelper(T firstParam, Rest ...rest)
    {
        DebugObj << firstParam;
        _AddInfo_ListHelper(rest...);
    }

    template<class T>
    inline void _unusedHelper(T value)
    {
        Q_UNUSED(value);
    }
    template<class T, class ...Rest>
    inline void _unusedHelper(T firstParam, Rest ...rest)
    {
        Q_UNUSED(firstParam);
        _unusedHelper(rest...);
    }

    template<class ...T>
    bool addInfo(T ...info)
    {
#if DEBUG_IS_ADDINFO == 1
        DebugStr.clear();
        _AddInfo_ListHelper(info...);
        return _AddInfo_inlineHelper(DebugStr);
#else
        _unusedHelper(info...);
        return false;
#endif
    }

} //namespace Debug

#endif
