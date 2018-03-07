#ifndef SCREENPLAY_H
#define SCREENPLAY_H

#include <QObject>

class ScreenPlay : public QObject
{
    Q_OBJECT
public:
    explicit ScreenPlay(QObject *parent = nullptr);

signals:

public slots:
};

#endif // SCREENPLAY_H