#include "settings.h"

Settings::Settings(QObject* parent)
    : QObject(parent)
{
    QFile configTmp;
    QJsonDocument configJsonDocument;
    QJsonParseError parseError;

    if (QDir(QString("config.json")).exists()) {
        configTmp.setFileName("config.json");
        configTmp.open(QIODevice::ReadOnly | QIODevice::Text);
        QString config = configTmp.readAll();
        configJsonDocument = QJsonDocument::fromJson(config.toUtf8(), &parseError);

        if ((parseError.error == QJsonParseError::NoError)) {
            //TODO
        }
    }
}

void Settings::setValue(const QString& key, const QVariant& value)
{
}

QVariant Settings::value(const QString& key, const QVariant& defaultValue) const
{
    QVariant a;
    return a;
}
