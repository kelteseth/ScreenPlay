#pragma once

#include <QApplication>

class MacIntegration : public QObject {
    Q_OBJECT
public:
    explicit MacIntegration(QObject* parent);
    void SetBackgroundLevel(QWindow* window);
};
