#ifndef MACINTEGRATION_H
#define MACINTEGRATION_H

#include <QApplication>


class MacIntegration : public QObject
{
    Q_OBJECT
public:
    explicit MacIntegration(QObject *parent);
    void SetBackgroundLevel(QWindow* window);
};

#endif // MACINTEGRATION_H
