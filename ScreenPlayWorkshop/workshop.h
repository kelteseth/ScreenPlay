#ifndef WORKSHOP_H
#define WORKSHOP_H

#include <QQuickItem>

class Workshop : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(Workshop)

public:
    Workshop(QQuickItem *parent = nullptr);
    ~Workshop();
};

#endif // WORKSHOP_H
