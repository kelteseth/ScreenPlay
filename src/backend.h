#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QDebug>

class Backend : public QObject
{
    Q_OBJECT
public:
    explicit Backend(QObject *parent = nullptr);

signals:
    void setTest();

public slots:
    void getTest();
    void exit();
};

#endif // BACKEND_H
