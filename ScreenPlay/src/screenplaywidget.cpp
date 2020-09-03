#include "screenplaywidget.h"

namespace ScreenPlay {

/*!
    \class ScreenPlay::ScreenPlayWidget
    \inmodule ScreenPlay
    \brief A Single Object to manage a Widget.

    This class is only for managing the QProcess to an extern ScreenPlayWidget!
*/

/*!
  \brief Constructor.
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
    {
        // If
        QJsonObject projectSettingsListModelProperties;

        if (properties.isEmpty()) {
            auto obj = Util::openJsonFileToObject(absolutePath + "/project.json");
            if (!obj)
                return;

            if (!obj->contains("properties"))
                return;
            projectSettingsListModelProperties = obj->value("properties").toObject();
        } else {
            projectSettingsListModelProperties = properties;
        }

        m_projectSettingsListModel.init(type, projectSettingsListModelProperties);
    }

    const QStringList proArgs {
        m_absolutePath,
        QString { "appID=" + m_appID },
        QVariant::fromValue(m_type).toString(),
        QString::number(m_position.x()),
        QString::number(m_position.y()),
    };

    m_process.setArguments(proArgs);
    m_process.setProgram(m_globalVariables->widgetExecutablePath().path());

    QObject::connect(&m_process, &QProcess::errorOccurred, this, [](QProcess::ProcessError error) {
        qDebug() << "error: " << error;
    });
    m_process.startDetached();
}

void ScreenPlayWidget::setSDKConnection(const std::shared_ptr<SDKConnection>& connection)
{
    m_connection = connection;
    qInfo() << "App widget connected!";
    QObject::connect(m_connection.get(), &SDKConnection::jsonMessageReceived, this, [this](const QJsonObject obj) {
        if (obj.value("messageType") == "positionUpdate") {
            setPosition({ obj.value("positionX").toInt(0), obj.value("positionY").toInt(0) });
            emit requestSave();
        }
    });

    // Check every X seconds if the widget is still alive
    QObject::connect(m_connection.get(), &SDKConnection::pingAliveReceived, this, [this]() {
        m_pingAliveTimer.stop();
        m_pingAliveTimer.start(16000);
    });

    QObject::connect(&m_pingAliveTimer, &QTimer::timeout, this, [this]() {
        qInfo() << "For " << m_pingAliveTimer.interval() << "ms no alive signal received. This means the Widget is dead and likely crashed!";
        emit requestClose(m_appID);
    });
    m_pingAliveTimer.start(16000);
}

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
