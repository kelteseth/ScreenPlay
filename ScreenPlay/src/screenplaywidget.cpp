// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlay/screenplaywidget.h"

namespace ScreenPlay {

/*!
    \class ScreenPlay::ScreenPlayWidget
    \inmodule ScreenPlay
    \brief A Single Object to manage a Widget.

    This class is only for managing the QProcess to an extern ScreenPlayWidget!
*/

/*!
  \brief Constructs a ScreenPlayWidget
*/
ScreenPlayWidget::ScreenPlayWidget(
    const QString& appID,
    const std::shared_ptr<GlobalVariables>& globalVariables,
    const QPoint& position,
    const QString& absolutePath,
    const QString& previewImage,
    const QJsonObject& properties,
    const InstalledType::InstalledType type)
    : QObject { nullptr }
    , m_globalVariables { globalVariables }
    , m_previewImage { previewImage }
    , m_appID { appID }
    , m_position { position }
    , m_type { type }
    , m_absolutePath { absolutePath }
{

    QJsonObject projectSettingsListModelProperties;

    if (properties.isEmpty()) {
        if (auto obj = ScreenPlayUtil::openJsonFileToObject(absolutePath + "/project.json")) {
            if (obj->contains("properties"))
                projectSettingsListModelProperties = obj->value("properties").toObject();
        }
    } else {
        projectSettingsListModelProperties = properties;
    }

    if (!projectSettingsListModelProperties.isEmpty())
        m_projectSettingsListModel.init(type, projectSettingsListModelProperties);

    m_appArgumentsList = QStringList {
        m_absolutePath,
        QString { "appID=" + m_appID },
        QVariant::fromValue(m_type).toString(),
        QString::number(m_position.x()),
        QString::number(m_position.y()),
    };
}

bool ScreenPlayWidget::start()
{
    m_process.setArguments(m_appArgumentsList);
    m_process.setProgram(m_globalVariables->widgetExecutablePath().toString());

    QObject::connect(&m_process, &QProcess::errorOccurred, this, [](QProcess::ProcessError error) {
        qDebug() << "error: " << error;
    });
    const bool success = m_process.startDetached();
    qInfo() << "Starting ScreenPlayWidget detached: " << (success ? "success" : "failed!");
    if (!success) {
        emit error(QString("Could not start Widget: " + m_process.errorString()));
        return false;
    }
    return success;
}

/*!
    \brief Sends command quit to the widget.
*/
void ScreenPlayWidget::close()
{
    // When the wallpaper never connected, this is invalid
    if (!m_connection) {
        qCritical() << "Cannot request quit, widget never connected!";
        return;
    }

    m_connection->close();
}

/*!
    \brief Connects to ScreenPlay. Start a alive ping check for every
           GlobalVariables::contentPingAliveIntervalMS  seconds.
*/
void ScreenPlayWidget::setSDKConnection(std::unique_ptr<SDKConnection> connection)
{
    m_connection = std::move(connection);
    qInfo() << "[3/3] SDKConnected (Widged) saved!";
    QObject::connect(m_connection.get(), &SDKConnection::jsonMessageReceived, this, [this](const QJsonObject obj) {
        if (obj.value("messageType") == "positionUpdate") {
            setPosition({ obj.value("positionX").toInt(0), obj.value("positionY").toInt(0) });
            emit requestSave();
            return;
        }
    });

    // Check every X seconds if the widget is still alive
    QObject::connect(m_connection.get(), &SDKConnection::pingAliveReceived, this, [this]() {
        m_pingAliveTimer.stop();
        m_pingAliveTimer.start(GlobalVariables::contentPingAliveIntervalMS);
    });

    QObject::connect(&m_pingAliveTimer, &QTimer::timeout, this, [this]() {
        qInfo() << "For " << m_pingAliveTimer.interval() << "ms no alive signal received. This means the Widget is dead and likely crashed!";
        emit requestClose(m_appID);
    });
    m_pingAliveTimer.start(GlobalVariables::contentPingAliveIntervalMS);
}

/*!
    \brief Loads the project.json content.
*/
QJsonObject ScreenPlayWidget::getActiveSettingsJson()
{
    QJsonObject obj;

    auto properties = m_projectSettingsListModel.getActiveSettingsJson();
    if (!properties.isEmpty())
        obj.insert("properties", properties);

    obj.insert("previewImage", m_previewImage);
    obj.insert("absolutePath", m_absolutePath);
    obj.insert("positionX", m_position.x());
    obj.insert("positionY", m_position.y());
    obj.insert("type", QVariant::fromValue(m_type).toString());
    return obj;
}
}
