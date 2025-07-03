// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlay/screenplaywidget.h"
#include "ScreenPlayCore/util.h"

#include <QCoreApplication>
#include <QDebug>

namespace ScreenPlay {

ScreenPlayWidget::ScreenPlayWidget(
    const QString& appID,
    const std::shared_ptr<GlobalVariables>& globalVariables,
    const WidgetData& widgetData,
    QObject* parent)
    : ScreenPlayExternalProcess(appID, globalVariables, widgetData, parent)
    , m_widgetData(widgetData)
{
    QJsonObject projectSettingsListModelProperties;

    if (widgetData.properties().isEmpty()) {
        if (auto obj = Util().openJsonFileToObject(widgetData.absolutePath() + "/project.json")) {
            if (obj->contains("properties"))
                projectSettingsListModelProperties = obj->value("properties").toObject();
        }
    } else {
        projectSettingsListModelProperties = widgetData.properties();
    }

    if (!projectSettingsListModelProperties.isEmpty()) {
        m_projectSettingsListModel = std::make_shared<ProjectSettingsListModel>();
        m_projectSettingsListModel->init(m_widgetData.type(), projectSettingsListModelProperties);
    }

    m_appArgumentsList = QStringList {
        "--projectpath", m_widgetData.absolutePath(),
        "--appID", m_appID,
        "--type", QVariant::fromValue(m_widgetData.type()).toString(),
        "--posX", QString::number(m_widgetData.position().x()),
        "--posY", QString::number(m_widgetData.position().y()),
        "--mainapppid", QString::number(m_processManager.getCurrentPID())
    };
}

bool ScreenPlayWidget::start()
{
    m_process.setArguments(m_appArgumentsList);
    m_process.setProgram(m_globalVariables->widgetExecutablePath().toString());

    const bool success = m_process.startDetached(&m_processID);
    qInfo() << "Starting ScreenPlayWidget detached:" << (success ? "success" : "failed!");

    if (!success) {
        emit error(QString("Could not start Widget: " + m_process.errorString()));
        return false;
    }
    emit processIDChanged(m_processID);
    return success;
}

QCoro::Task<Result> ScreenPlayWidget::close()
{
    if (!m_connection) {
        qCritical() << "Cannot request quit, widget never connected!";
        co_return Result { true, {}, "Widget was never connected" };
    }

    if (!m_connection->close()) {
        co_return Result { false, {}, "Failed to close widget connection" };
    }

    co_return Result { true, {}, "Widget closed successfully" };
}

void ScreenPlayWidget::setupSDKConnection()
{
    ScreenPlayExternalProcess::setupSDKConnection();

    if (!m_connection) {
        return;
    }

    qInfo() << "[3/3] SDKConnected (Widget) saved!";

    QObject::connect(m_connection.get(), &SDKConnection::jsonMessageReceived, this, [this](const QJsonObject obj) {
        if (obj.value("messageType") == "positionUpdate") {
            setPosition({ obj.value("positionX").toInt(0), obj.value("positionY").toInt(0) });
            emit requestSave();
            return;
        }
    });

    QObject::connect(m_connection.get(), &SDKConnection::pingAliveReceived, this, [this]() {
        m_pingAliveTimer.stop();
        m_pingAliveTimer.start(GlobalVariables::contentPingAliveIntervalMS);
        std::optional<bool> running = m_processManager.isRunning(m_processID);
        if (running.has_value()) {
            // Process is running normally
        } else {
            qInfo() << "Widget process" << m_processID << "not found - widget may have terminated";
            handleTimeoutOrCrash();
        }
    });

    QObject::connect(&m_pingAliveTimer, &QTimer::timeout, this, [this]() {
        qInfo() << "For" << m_pingAliveTimer.interval() << "ms no alive signal received. This means the Widget is dead and likely crashed!";
        handleTimeoutOrCrash();
    });
    m_pingAliveTimer.start(GlobalVariables::contentPingAliveIntervalMS);
}

QJsonObject ScreenPlayWidget::getActiveSettingsJson()
{
    QJsonObject obj;

    if (m_projectSettingsListModel) {
        auto properties = m_projectSettingsListModel->getActiveSettingsJson();
        if (!properties.isEmpty())
            obj.insert("properties", properties);
    }

    obj.insert("previewImage", m_widgetData.previewImage());
    obj.insert("absolutePath", m_widgetData.absolutePath());
    obj.insert("positionX", m_widgetData.position().x());
    obj.insert("positionY", m_widgetData.position().y());
    obj.insert("type", QVariant::fromValue(m_widgetData.type()).toString());
    return obj;
}

void ScreenPlayWidget::setPosition(QPoint position)
{
    if (m_widgetData.position() == position)
        return;

    m_widgetData.setPosition(position);
    emit positionChanged(m_widgetData.position());
}

}

#include "moc_screenplaywidget.cpp"
