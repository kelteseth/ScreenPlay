#pragma once

#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QPoint>
#include <QProcess>

#include <memory>

#include "globalvariables.h"

namespace ScreenPlay {

using std::shared_ptr,
    std::make_shared;

class ScreenPlayWidget : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString projectPath READ projectPath WRITE setProjectPath NOTIFY projectPathChanged)
    Q_PROPERTY(QString previewImage READ previewImage WRITE setPreviewImage NOTIFY previewImageChanged)
    Q_PROPERTY(QPoint position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)

public:
    explicit ScreenPlayWidget(
        const QString& appID,
        const shared_ptr<GlobalVariables>& globalVariables,
        const QString& projectPath,
        const QString& previewImage,
        const QString& fullPath,
        QObject* parent = nullptr);

    ~ScreenPlayWidget() {}

    QString projectPath() const
    {
        return m_projectPath;
    }

    QString previewImage() const
    {
        return m_previewImage;
    }

    QPoint position() const
    {
        return m_position;
    }

    QString appID() const
    {
        return m_appID;
    }

public slots:
    void setProjectPath(QString projectPath)
    {
        if (m_projectPath == projectPath)
            return;

        m_projectPath = projectPath;
        emit projectPathChanged(m_projectPath);
    }

    void setPreviewImage(QString previewImage)
    {
        if (m_previewImage == previewImage)
            return;

        m_previewImage = previewImage;
        emit previewImageChanged(m_previewImage);
    }

    void setPosition(QPoint position)
    {
        if (m_position == position)
            return;

        m_position = position;
        emit positionChanged(m_position);
    }

    void setAppID(QString appID)
    {
        if (m_appID == appID)
            return;

        m_appID = appID;
        emit appIDChanged(m_appID);
    }

signals:
    void projectPathChanged(QString projectPath);
    void previewImageChanged(QString previewImage);
    void positionChanged(QPoint position);
    void appIDChanged(QString appID);

private:
    QProcess m_process;
    const shared_ptr<GlobalVariables>& m_globalVariables;

    QString m_projectPath;
    QString m_previewImage;
    QString m_appID;
    QPoint m_position;
};
}
