#include "settings.h"

Settings::Settings(QObject* parent)
    : QObject(parent)
{
    QFile configTmp;
    QJsonDocument configJsonDocument;
    QJsonParseError parseError;
    QJsonObject configObj;

    configTmp.setFileName(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/settings.json");

    if (!configTmp.exists()) {
        //If we cannot find the settings json file we will create one
        createDefaultConfig();
    }
    configTmp.open(QIODevice::ReadOnly | QIODevice::Text);
    QString config = configTmp.readAll();
    configJsonDocument = QJsonDocument::fromJson(config.toUtf8(), &parseError);

    if (!(parseError.error == QJsonParseError::NoError)) {
        return;
    }

    configObj = configJsonDocument.object();
    m_version = configObj.value("version").toVariant();
    m_autostart = configObj.value("autostart").toBool();
    m_highPriorityStart = configObj.value("highPriorityStart").toBool();
    m_sendStatistics = configObj.value("sendStatistics").toBool();
    m_renderer = static_cast<Renderer>(configObj.value("renderer-value").toInt());
}

void Settings::createDefaultConfig()
{
    Q_INIT_RESOURCE(qml);

    QFile file(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/settings.json");
    QFile defaultSettings(":/settings.json");

    file.open(QIODevice::WriteOnly | QIODevice::Text);
    defaultSettings.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream out(&file);
    QTextStream defaultOut(&defaultSettings);

    out << defaultOut.readAll();

    file.close();
}
