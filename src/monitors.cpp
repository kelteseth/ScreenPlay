#include "monitors.h"

Monitors::Monitors(QObject* parent)
    : QObject(parent)
{
loadScreens();
}

void Monitors::loadScreens()
{
    for (int i = 0; i < QApplication::screens().count(); i++) {
        QScreen* screen = QApplication::screens().at(i);
        qDebug() << screen->geometry();
        _screen.append(QApplication::screens().at(i));

        _monitors.append(Monitor(screen->name(), screen->size(), screen->availableGeometry(), i, false));
    }
}

QVariantList Monitors::get()
{
    QVariantList list;

    return list;
}

Monitor::Monitor(QString name, QSize size, QRect availableGeometry, int number, bool isVirtualDesktop)
{
    _name = name;
    _size = size;
    _availableGeometry = availableGeometry;
    _number = number;
    _isVirtualDesktop = isVirtualDesktop;
}
