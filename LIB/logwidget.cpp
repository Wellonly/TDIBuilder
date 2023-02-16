#include "logwidget.h"

#include <QDateTime>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QVBoxLayout>

LogWidget::LogWidget(QWidget *parent) : QWidget(parent)
{
    setObjectName("LogWidget");
    setWindowTitle("Log");

    setMinimumSize(150,50);

    tree = new QTreeWidget(this);
    tree->setRootIsDecorated(false);
    tree->setColumnCount(1);
    tree->setHeaderHidden(true);

    connect(tree, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(clearSlot(QModelIndex)));

    QVBoxLayout* lout = new QVBoxLayout(this);
    lout->addWidget(tree);
    setLayout(lout);
}

LogWidget::~LogWidget()
{
    delete tree;
}

LogWidget::LogWidget(const LogWidget &other) : QWidget(0), tree(other.tree)
{

}

void LogWidget::addInfo(QString info)
{
    QTreeWidgetItem *newNode = new QTreeWidgetItem();
    newNode->setText(0, QDateTime::currentDateTime().toString(QLatin1String(LOGTIMEFORMATER)) + info);
    newNode->setIcon(0, QIcon(":/info.png"));
    tree->addTopLevelItem(newNode);

#ifdef DEBUG_OUT_TO_STD /* not tested... */
    QTextStream out(DEBUG_OUT_TO_STD);
    out << newNode->text(0) << "\n";
#endif

#ifdef DEBUG_OUT_TO_FILE /*TODO: file as static var */
    QFile file(DEBUG_OUT_TO_FILE);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        return;
    }
    QTextStream out(&file);
    out << newNode->text(0) << "\n";
    file.close();
#endif
}

void LogWidget::addInfo(QVariant usrData)
{
    this->addInfo(usrData.toString());
}

void LogWidget::clearSlot(QModelIndex)
{
    if (tree->topLevelItemCount()) {
        tree->clear();
    }
    else
    {
        addInfo(QStringLiteral("Don't click anywhere!!!"));
    }
}

