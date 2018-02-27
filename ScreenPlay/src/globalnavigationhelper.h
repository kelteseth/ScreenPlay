#pragma once

#include <QObject>
#include <QString>

class GlobalNavigationHelper : public QObject {
    Q_OBJECT
public:
    explicit GlobalNavigationHelper(QObject* parent = nullptr);

signals:
    void requestNavigation(QString nav);

public slots:
    void setNavigation(QString nav);
};


