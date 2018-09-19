#ifndef SYSINFO_H
#define SYSINFO_H

#include <QQuickItem>

class SysInfo : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(SysInfo)

public:
    SysInfo(QQuickItem *parent = nullptr);
    ~SysInfo();
};

#endif // SYSINFO_H
