#ifndef SCREENPLAYSETTINGS_H
#define SCREENPLAYSETTINGS_H

#include <QObject >
#include <QQmlPropertyMap>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QVariant>
#include <QDir>
#include <QFile>

class Settings : public QObject {
    Q_OBJECT
public:
    explicit Settings(QObject* parent = nullptr);

    Q_INVOKABLE void setValue(const QString& key, const QVariant& value);
    Q_INVOKABLE QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;
    Q_PROPERTY(QQmlPropertyMap* globalSetting READ globalSetting WRITE setglobalSetting NOTIFY globalSettingChanged)

    QQmlPropertyMap* globalSetting() const
    {
        return m_globalSetting;
    }

signals:

    void globalSettingChanged(QQmlPropertyMap* globalSetting);

public slots:

    void setglobalSetting(QQmlPropertyMap* globalSetting)
    {
        if (m_globalSetting == globalSetting)
            return;

        m_globalSetting = globalSetting;
        emit globalSettingChanged(m_globalSetting);
    }

private:
    QQmlPropertyMap* m_globalSetting;
};

#endif // SCREENPLAYSETTINGS_H
