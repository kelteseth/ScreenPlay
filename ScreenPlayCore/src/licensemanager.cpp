#include "licensemanager.h"
#include "ScreenPlaySysInfo/cpu.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QTextStream>
#include <QVariant>

namespace ScreenPlay {

LicenseManager::LicenseManager(QObject* parent)
    : QObject(parent)
{
}

bool LicenseManager::checkLocalLicense()
{
    CPU cpu;
    cpu.refreshCPUInfo();
    QString userName = qgetenv("USERNAME");
    QString licenseKey = generateLicenseKey(cpu.name(), userName);
    return verifyLicenseFile(licenseKey);
}

QString LicenseManager::generateLicenseKey(const QString& cpuName, const QString& userName)
{
    QString combined = cpuName + "|" + userName + "|" + QVariant::fromValue(ScreenPlayEnums::Version::OpenSourceProSteam).toString();
    QByteArray hash = QCryptographicHash::hash(combined.toUtf8(), QCryptographicHash::Sha256);
    return hash.toHex();
}

QString LicenseManager::getLicenseFilePath() const
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(path);
    return path + "/license.json";
}

bool LicenseManager::saveLicenseFile(const QString& licenseKey)
{
    QJsonObject licenseData;
    licenseData["configVersion"] = "1.0.0";
    licenseData["licenseKey"] = licenseKey;
    licenseData["licenseVersion"] = QVariant::fromValue(m_version).toString();
    licenseData["licenseHash"] = generateLicenseHash();

    QJsonDocument doc(licenseData);
    QFile file(getLicenseFilePath());

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open license file for writing";
        return false;
    }

    return file.write(doc.toJson()) != -1;
}

bool LicenseManager::verifyLicenseFile(const QString& expectedKey)
{
    QFile file(getLicenseFilePath());
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull()) {
        qWarning() << "Invalid JSON in license file";
        return false;
    }

    QJsonObject licenseData = doc.object();
    QString storedConfigVersion = licenseData["version"].toString();
    QString storedKey = licenseData["licenseKey"].toString();
    QString storedVersion = licenseData["licenseVersion"].toString();
    QString storedHash = licenseData["licenseHash"].toString();

    // Verify license key
    if (storedKey != expectedKey) {
        return false;
    }

    // Verify version
    QVariant storedVersionVar = QVariant(storedVersion);
    ScreenPlay::ScreenPlayEnums::Version version;
    bool ok = storedVersionVar.convert(QMetaType::fromType<ScreenPlay::ScreenPlayEnums::Version>());
    if (!ok || storedVersionVar.value<ScreenPlay::ScreenPlayEnums::Version>() != m_version) {
        return false;
    }

    // Verify hash
    return storedHash == generateLicenseHash();
}

QString LicenseManager::generateLicenseHash() const
{
    CPU cpu;
    cpu.refreshCPUInfo();
    QString userName = qgetenv("USERNAME");
    QString combined = cpu.name() + "|" + userName + "|" + QVariant::fromValue(m_version).toString();
    return QCryptographicHash::hash(combined.toUtf8(), QCryptographicHash::Sha256).toHex();
}

}
