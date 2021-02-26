#pragma once
#include <QObject>
#include <QTimer>

class Uptime : public QObject {
    Q_OBJECT

    Q_PROPERTY(int days READ days WRITE setDays NOTIFY daysChanged)
    Q_PROPERTY(int years READ years WRITE setYears NOTIFY yearsChanged)
    Q_PROPERTY(int hours READ hours WRITE setHours NOTIFY hoursChanged)
    Q_PROPERTY(int minutes READ minutes WRITE setMinutes NOTIFY minutesChanged)
    Q_PROPERTY(int seconds READ seconds WRITE setSeconds NOTIFY secondsChanged)

public:
    Uptime(QObject* parent = nullptr);

    int days() const { return m_days; }
    int years() const { return m_years; }
    int hours() const { return m_hours; }
    int minutes() const { return m_minutes; }
    int seconds() const { return m_seconds; }

public slots:
    void update();

    void setDays(int days)
    {
        if (m_days == days)
            return;

        m_days = days;
        emit daysChanged(m_days);
    }
    void setYears(int years)
    {
        if (m_years == years)
            return;

        m_years = years;
        emit yearsChanged(m_years);
    }

    void setHours(int hours)
    {
        if (m_hours == hours)
            return;

        m_hours = hours;
        emit hoursChanged(m_hours);
    }

    void setMinutes(int minutes)
    {
        if (m_minutes == minutes)
            return;

        m_minutes = minutes;
        emit minutesChanged(m_minutes);
    }

    void setSeconds(int seconds)
    {
        if (m_seconds == seconds)
            return;

        m_seconds = seconds;
        emit secondsChanged(m_seconds);
    }

signals:
    void daysChanged(int days);
    void yearsChanged(int years);
    void hoursChanged(int hours);
    void minutesChanged(int minutes);
    void secondsChanged(int seconds);

private:
    int m_days = 0;
    int m_years = 0;
    int m_hours = 0;
    int m_minutes = 0;
    int m_seconds = 0;

    int m_tickRate = 1000;
    QTimer m_updateTimer;
};
