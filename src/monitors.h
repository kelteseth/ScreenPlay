#ifndef MONITORS_H
#define MONITORS_H

#include <QApplication>
#include <QDebug>
#include <QList>
#include <QObject>
#include <QRect>
#include <QScreen>
#include <QSize>
#include <QVariantList>

class Monitor;

class Monitors : public QObject {
    Q_OBJECT


public:
    explicit Monitors(QObject* parent = nullptr);

    Q_INVOKABLE void loadScreens();
    Q_INVOKABLE QVariantList get();

signals:

public slots:

private:
    QList<Monitor> _monitors;
    QList<QScreen *> _screen;
    int primaryScreen;
};

class Monitor {

public:
    Monitor(QString name, QSize size, QRect availableGeometry, int number, bool isVirtualDesktop);

    QString _name;
    QSize _size;
    QRect _availableGeometry;
    int _number;
    bool _isVirtualDesktop;
    QScreen* _screen = nullptr;
};

#endif // MONITORS_H
