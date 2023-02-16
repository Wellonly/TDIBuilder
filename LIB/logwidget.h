#ifndef LOGWIDGET_H
#define LOGWIDGET_H
#include "app_config.h" //before 20151216 was: "config.h"

#ifndef LOGTIMEFORMATER
#define LOGTIMEFORMATER "yy/MM/dd HH:mm:ss.zzz: "
#endif

#include <QTreeWidget>
#include <QWidget>

class LogWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LogWidget(QWidget *parent = 0);
    ~LogWidget();
    LogWidget(const LogWidget& other);
signals:

public slots:
    void addInfo(QString info);
    void addInfo(QVariant usrData);

    void clearSlot(QModelIndex);

private:
    QTreeWidget* tree;
};
Q_DECLARE_METATYPE(LogWidget)
Q_DECLARE_METATYPE(LogWidget*)

#endif // LOGWIDGET_H
