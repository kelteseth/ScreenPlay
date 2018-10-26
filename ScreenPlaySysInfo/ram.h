#pragma once

#include <QObject>
#include <QDebug>

class RAM : public QObject {
    Q_OBJECT
    Q_PROPERTY(float usage READ usage NOTIFY usageChanged)

public:
    explicit RAM(QObject* parent = nullptr);

    float usage() const
    {
        return m_usage;
    }

signals:

    void usageChanged(float usage);

public slots:
    void exe(){
        qDebug() << "aa";
    }
    void setUsage(float usage)
    {
        qWarning("Floating point comparison needs context sanity check");
        if (qFuzzyCompare(m_usage, usage))
            return;

        m_usage = usage;
        emit usageChanged(m_usage);
    }

private:
    float m_usage = 42.0f;
};

