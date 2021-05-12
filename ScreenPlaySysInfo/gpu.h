#pragma once
#include <QObject>

class GPU : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString vendor READ vendor WRITE setVendor NOTIFY vendorChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int ramSize READ ramSize WRITE setRamSize NOTIFY ramSizeChanged)
    Q_PROPERTY(int cacheSize READ cacheSize WRITE setCacheSize NOTIFY cacheSizeChanged)
    Q_PROPERTY(int maxFrequency READ maxFrequency WRITE setMaxFrequency NOTIFY maxFrequencyChanged)

public:
    explicit GPU(QObject* parent = nullptr);
    const QString& vendor() const { return m_vendor; }
    const QString& name() const { return m_name; }
    int ramSize() const { return m_ramSize; }
    int cacheSize() const { return m_cacheSize; }
    int maxFrequency() const { return m_maxFrequency; }

public slots:
    void setVendor(const QString& vendor)
    {
        if (m_vendor == vendor)
            return;
        m_vendor = vendor;
        emit vendorChanged(m_vendor);
    }
    void setName(const QString& name)
    {
        if (m_name == name)
            return;
        m_name = name;
        emit nameChanged(m_name);
    }

    void setRamSize(int ramSize)
    {
        if (m_ramSize == ramSize)
            return;
        m_ramSize = ramSize;
        emit ramSizeChanged(m_ramSize);
    }

    void setCacheSize(int cacheSize)
    {
        if (m_cacheSize == cacheSize)
            return;
        m_cacheSize = cacheSize;
        emit cacheSizeChanged(m_cacheSize);
    }

    void setMaxFrequency(int maxFrequency)
    {
        if (m_maxFrequency == maxFrequency)
            return;
        m_maxFrequency = maxFrequency;
        emit maxFrequencyChanged(m_maxFrequency);
    }

signals:
    void vendorChanged(const QString& vendor);
    void nameChanged(const QString& name);
    void ramSizeChanged(int ramSize);
    void cacheSizeChanged(int cacheSize);
    void maxFrequencyChanged(int maxFrequency);

private:
    QString m_vendor;
    QString m_name;
    int m_ramSize = 0;
    int m_cacheSize = 0;
    int m_maxFrequency = 0;
};
