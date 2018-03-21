#include "spwidgetmainwindow.h"

SPWidgetmainwindow::SPWidgetmainwindow(QString projectPath, QString appid, QScreen* parent)
    : QWindow(parent)
{
    m_appID = appid;

    QFile configTmp;
    QJsonDocument configJsonDocument;
    QJsonParseError parseError;

    configTmp.setFileName(projectPath + "/project.json");
    configTmp.open(QIODevice::ReadOnly | QIODevice::Text);
    m_projectConfig = configTmp.readAll();
    configJsonDocument = QJsonDocument::fromJson(m_projectConfig.toUtf8(), &parseError);

    if (!(parseError.error == QJsonParseError::NoError)) {
        qWarning("Settings Json Parse Error ");
    }

    m_project = configJsonDocument.object();

    emit setWidgetSource(projectPath + "/" + m_project.value("file").toString());

    m_quickRenderer = QSharedPointer<QQuickView>(new QQuickView(this));
    m_quickRenderer.data()->rootContext()->setContextProperty("mainwindow", this);
}
