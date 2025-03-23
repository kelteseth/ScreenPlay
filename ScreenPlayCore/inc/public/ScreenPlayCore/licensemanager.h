#pragma once
#include "ScreenPlayCore/globalenums.h"
#include <QObject>
#include <QString>

namespace ScreenPlay {

class LicenseManager : public QObject {
    Q_OBJECT
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
public:
    explicit LicenseManager(QObject* parent = nullptr);
    bool checkLocalLicense();

    ScreenPlayEnums::Version getVersion() const { return m_version; }
    void setVersion(ScreenPlayEnums::Version version);

private:
    QString generateLicenseKey(const QString& cpuName, const QString& userName);
    bool saveLicenseFile(const QString& licenseKey);
    bool analyzeLicenseFile(const QMap<ScreenPlayEnums::Version, QString>& possibleKeys);
    QString getLicenseFilePath() const;
    QString generateLicenseHash() const;

private:
    ScreenPlayEnums::Version m_version = ScreenPlayEnums::Version::OpenSourceStandalone;
};

}