/// Debug informer for Qt Designer
/// Copyright 2015; Author: Valentin Zotov
/// Support: wellmanmail@gmail.com

#include "debuginfo.h"

#include <QMainWindow>

DebugInfo::DebugInfo(QDesignerFormEditorInterface *formEditor)
:QDockWidget(QLatin1String("Debug"), formEditor->topLevel())
,m_formEditor(formEditor)
,logWidget(new LogWidget(this))
,debLayout(new QVBoxLayout(this))
{
    Q_ASSERT(formEditor != 0);
    setObjectName(QLatin1String("DebugInformer"));
    setVisible(true);
    setAllowedAreas(Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

    debLayout->addWidget(logWidget);
    setLayout(debLayout);

    addInfoAction = new QAction(QIcon(":/Well.png"), DEBUG_ACTION_NAME, formEditor->topLevel());
    connect(addInfoAction, SIGNAL(triggered(bool)), this, SLOT(debugActionTriggered(bool)));
    formEditor->topLevel()->addAction(addInfoAction);

    setWidget(logWidget);

    QMainWindow* mwindow = qobject_cast<QMainWindow*>(formEditor->topLevel());
    if (mwindow) mwindow->addDockWidget(Qt::RightDockWidgetArea, this);

    show();
}

DebugInfo::DebugInfo(const DebugInfo &other) : QDockWidget(0)
  ,logWidget(other.logWidget)
{
}

DebugInfo::~DebugInfo()
{
    delete logWidget;
    delete debLayout;
}

void DebugInfo::debugActionTriggered(bool)
{
    addInfo(addInfoAction->data());
}

void DebugInfo::addInfo(QVariant usrData)
{
    logWidget->addInfo(usrData.toString());
}

///////// Debug... ///////////////////////////////////////////////////////////////

