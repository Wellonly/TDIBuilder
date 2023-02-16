#ifndef LINKEDDATA_H
#define LINKEDDATA_H

#include "tdiclasses.h"
#include <QWidget>

class LinkedData : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(SmoRecord::SmoDataType DataType READ smoDataType WRITE setSmoDataType)
    Q_PROPERTY(QString Data READ linkAddress WRITE setLinkAddress)

public:
    explicit LinkedData(QWidget *parent = 0);
    LinkedData(const LinkedData& other);
    ~LinkedData();

    QString linkAddress() const;
    void setLinkAddress(const QString &linkAddress);

    SmoDataType smoDataType() const;

signals:

public slots:

    void setSmoDataType(SmoDataType smoDataType);

private:
    SmoDataType m_smoDataType;
    QString m_linkAddress;
};
Q_DECLARE_METATYPE(LinkedData)
Q_DECLARE_METATYPE(LinkedData*)

#endif // LINKEDDATA_H
