#pragma once

#include <QApplication>

class MacBridge : public QObject
{
    Q_OBJECT

private:
    static MacBridge* bridge;
    MacBridge();
public:
    static MacBridge* instance();
    static void SetBackgroundLevel(QWindow* window);
};
