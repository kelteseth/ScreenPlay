#ifndef SCREENPLAYITEM_H
#define SCREENPLAYITEM_H

#include <QObject>
#include <QVariant>

class ScreenPlayItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariant name READ name WRITE setName NOTIFY nameChanged);


    QVariant m_name;

public:
    explicit ScreenPlayItem(QObject *parent = 0);

QVariant name() const
{
    return m_name;
}

signals:

void nameChanged(QVariant name);

public slots:
void setName(QVariant name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged(name);
}
};

#endif // SCREENPLAYITEM_H
