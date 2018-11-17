#ifndef WORKSHOP_H
#define WORKSHOP_H

#include <QQuickItem>
#include <QtDebug>
#include "aimberapi.h"
class Workshop : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(Workshop)

public:
    Workshop(QQuickItem *parent = nullptr);
    ~Workshop();

public slots:
    void hello(){
       qDebug() << "bla";
    }
signals:
private:
    AimberAPI m_aimberAPI;
};

#endif // WORKSHOP_H
