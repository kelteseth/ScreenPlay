#include "licensemanager.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QSysInfo>
#include <QTextStream>
#include <QVariant>

namespace ScreenPlay {

LicenseManager::LicenseManager(QObject* parent)
    : QObject(parent)
{
}

bool LicenseManager::checkLocalLicense()
{
    QByteArray machineId = QSysInfo::machineUniqueId();
    QString userName = qgetenv("USERNAME");

    // Check if we got a valid machine ID
    if (machineId.isEmpty()) {
        qFatal("Unable to load QSysInfo::machineUniqueId");
    }

    // Convert the byte array to a string for license key generation
    QString machineIdStr = machineId.toHex();

    // Generate keys for all possible versions to check which one matches
    QMap<ScreenPlayEnums::Version, QString> possibleKeys;
    ScreenPlayEnums::Version originalVersion = m_version;

    // Store the current version
    possibleKeys[m_version] = generateLicenseKey(machineIdStr, userName);

    // Try all possible versions
    m_version = ScreenPlayEnums::Version::OpenSourceStandalone;
    possibleKeys[m_version] = generateLicenseKey(machineIdStr, userName);

    m_version = ScreenPlayEnums::Version::OpenSourceSteam;
    possibleKeys[m_version] = generateLicenseKey(machineIdStr, userName);

    m_version = ScreenPlayEnums::Version::OpenSourceProStandalone;
    possibleKeys[m_version] = generateLicenseKey(machineIdStr, userName);

    m_version = ScreenPlayEnums::Version::OpenSourceProSteam;
    possibleKeys[m_version] = generateLicenseKey(machineIdStr, userName);

    m_version = ScreenPlayEnums::Version::OpenSourceUltraStandalone;
    possibleKeys[m_version] = generateLicenseKey(machineIdStr, userName);

    m_version = ScreenPlayEnums::Version::OpenSourceUltraSteam;
    possibleKeys[m_version] = generateLicenseKey(machineIdStr, userName);

    // Restore original version
    m_version = originalVersion;

    // Analyze the license file and determine which version it's for
    return analyzeLicenseFile(possibleKeys);
}

QString LicenseManager::generateLicenseKey(const QString& machineId, const QString& userName)
{
    QString combined = machineId + "|" + userName + "|" + QVariant::fromValue(m_version).toString();
    QByteArray key = QCryptographicHash::hash(combined.toUtf8(), QCryptographicHash::Sha256);
    return key.toHex();
}

// Rest of the class implementation remains the same...

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

    QJsonDocument doc(licenseData);
    QFile file(getLicenseFilePath());

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open license file for writing";
        return false;
    }

    return file.write(doc.toJson()) != -1;
}

bool LicenseManager::analyzeLicenseFile(const QMap<ScreenPlayEnums::Version, QString>& possibleKeys)
{
    QFile file(getLicenseFilePath());
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "License file not found";
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull()) {
        qWarning() << "Invalid JSON in license file";
        return false;
    }

    QJsonObject licenseData = doc.object();
    // We bump the version when we (ever) add a new dlc, to force
    // a recheck
    QString storedConfigVersion = licenseData["configVersion"].toString();
    QString storedKey = licenseData["licenseKey"].toString();
    QString storedVersion = licenseData["licenseVersion"].toString();

    // Output the stored license key
    qDebug() << "Stored license key:" << storedKey;
    qDebug() << "Stated license version in file:" << storedVersion;

    // Check which version this key is valid for
    ScreenPlayEnums::Version detectedVersion = ScreenPlayEnums::Version::OpenSourceStandalone;
    bool foundValidVersion = false;

    // Iterate through all possible versions to see which one the key is valid for
    QMap<ScreenPlayEnums::Version, QString>::const_iterator i;
    for (i = possibleKeys.constBegin(); i != possibleKeys.constEnd(); ++i) {
        if (i.value() == storedKey) {
            detectedVersion = i.key();
            foundValidVersion = true;
            qDebug() << "Key matches version:" << QVariant::fromValue(detectedVersion).toString();
            break;
        }
    }

    if (!foundValidVersion) {
        qWarning() << "License key does not match any valid version for this user/machine";
        return false;
    }

    // Check if the stored version matches the detected version
    QVariant storedVersionVar = QVariant(storedVersion);
    ScreenPlayEnums::Version parsedStoredVersion;
    bool ok = storedVersionVar.convert(QMetaType::fromType<ScreenPlay::ScreenPlayEnums::Version>());

    if (ok && storedVersionVar.value<ScreenPlayEnums::Version>() != detectedVersion) {
        qWarning() << "Stored version does not match the version for which this key is valid";
        qDebug() << "Stored version: " << storedVersion;
        qDebug() << "Actual key version: " << QVariant::fromValue(detectedVersion).toString();
    }

    // Set the actual detected version
    m_version = detectedVersion;

    return true;
}

QString LicenseManager::generateLicenseHash() const
{
    // Implementation for license hash generation
    QString combined = QVariant::fromValue(m_version).toString();
    QByteArray hash = QCryptographicHash::hash(combined.toUtf8(), QCryptographicHash::Sha256);
    return hash.toHex();
}

void LicenseManager::setVersion(ScreenPlayEnums::Version version)
{
    if (m_version == version) {
        return; // No change, do nothing
    }

    m_version = version;

    // Generate a new license key and save it
    QByteArray machineId = QSysInfo::machineUniqueId();
    QString userName = qgetenv("USERNAME");

    if (machineId.isEmpty()) {
        qWarning() << "Failed to get machine ID when updating license";
        return;
    }

    QString machineIdStr = machineId.toHex();
    QString licenseKey = generateLicenseKey(machineIdStr, userName);

    if (saveLicenseFile(licenseKey)) {
        qInfo() << "License updated for version:" << QVariant::fromValue(m_version).toString();
    } else {
        qWarning() << "Failed to save license file for version:" << QVariant::fromValue(m_version).toString();
    }
}

}