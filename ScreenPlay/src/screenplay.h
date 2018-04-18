#pragma once

#include <QCryptographicHash>
#include <QGuiApplication>
#include <QObject>
#include <QPoint>
#include <QProcess>
#include <QRandomGenerator>
#include <QSharedPointer>
#include <QVector>

#include "installedlistmodel.h"
#include "monitorlistmodel.h"
#include "projectfile.h"
#include "projectsettingslistmodel.h"
#include "sdkconnector.h"
#include "settings.h"

/*!
    \class ScreenPlay
    \brief Used for Creation of Wallpaper, Scenes and Widgets
*/

class ScreenPlayWallpaper;
class ScreenPlayWidget;

class ScreenPlay : public QObject {
    Q_OBJECT
public:
    explicit ScreenPlay(InstalledListModel* ilm, Settings* set, MonitorListModel* mlm, QGuiApplication* qGuiApplication, SDKConnector* sdkc, QObject* parent = nullptr);

    Settings* settings() const;

    InstalledListModel* m_ilm;
    Settings* m_settings;
    MonitorListModel* m_mlm;
    QGuiApplication* m_qGuiApplication;
    SDKConnector* m_sdkc;

signals:
    void allWallpaperRemoved();
    void projectSettingsListModelFound(ProjectSettingsListModel* li);
    void projectSettingsListModelNotFound();

public slots:
    void createWallpaper(int monitorIndex, QUrl absoluteStoragePath, QString previewImage, float volume, QString fillMode);
    void createWidget(QUrl absoluteStoragePath, QString previewImage);
    void removeAllWallpaper();
    void requestProjectSettingsListModelAt(int index);
    void setWallpaperValue(int at, QString key, QString value);
    void removeWallpaperAt(int at);
    QVector<int> getMonitorByAppID(QString appID);
    QString generateID();

private:
    QVector<QSharedPointer<ScreenPlayWallpaper>> m_screenPlayWallpaperList;
    QVector<QSharedPointer<ScreenPlayWidget>> m_screenPlayWidgetList;
};

class ScreenPlayWallpaper : public QObject {
    Q_OBJECT

    Q_PROPERTY(QVector<int> screenNumber READ screenNumber WRITE setScreenNumber NOTIFY screenNumberChanged)
    Q_PROPERTY(QString projectPath READ projectPath WRITE setProjectPath NOTIFY projectPathChanged)
    Q_PROPERTY(QString previewImage READ previewImage WRITE setPreviewImage NOTIFY previewImageChanged)
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)

public:
    explicit ScreenPlayWallpaper(QVector<int> screenNumber, QString projectPath, QString previewImage, float volume, QString fillMode, ScreenPlay* parent)
    {
        m_screenNumber = screenNumber;
        m_projectPath = projectPath;
        m_previewImage = previewImage;

        // We do not want to parent the QProcess because the
        // Process manages its lifetime and destructing (animation) itself
        // via a disconnection from the ScreenPlay SDK
        QProcess* m_process = new QProcess();

        connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [=](int exitCode, QProcess::ExitStatus exitStatus) {
            qDebug() << "EX: " << exitCode;
        });
        connect(m_process, &QProcess::errorOccurred, [=](QProcess::ProcessError error) {
            qDebug() << "EX: " << error;
        });

        QStringList proArgs;
        proArgs.append(QString::number(m_screenNumber.at(0)));
        proArgs.append(m_projectPath);
        m_appID = parent->generateID();
        proArgs.append("appID="+m_appID);
        proArgs.append(parent->m_settings->decoder());
        proArgs.append(QString::number(volume));
        proArgs.append(fillMode);
        m_process->setArguments(proArgs);
        m_process->setProgram(parent->settings()->screenPlayWindowPath().toString());
        m_process->start();
        m_projectSettingsListModel = QSharedPointer<ProjectSettingsListModel>(new ProjectSettingsListModel(projectPath + "/project.json"));
    }

    QSharedPointer<ProjectSettingsListModel> projectSettingsListModel() const;

    QVector<int> screenNumber() const
    {
        return m_screenNumber;
    }

    QString projectPath() const
    {
        return m_projectPath;
    }

    QString previewImage() const
    {
        return m_previewImage;
    }

    QString appID() const
    {
        return m_appID;
    }

signals:
    void screenNumberChanged(QVector<int> screenNumber);
    void projectPathChanged(QString projectPath);
    void previewImageChanged(QString previewImage);
    void projectSettingsListModelAt(ProjectSettingsListModel* li);
    void appIDChanged(QString appID);

public slots:

    void setScreenNumber(QVector<int> screenNumber)
    {
        if (m_screenNumber == screenNumber)
            return;

        m_screenNumber = screenNumber;
        emit screenNumberChanged(m_screenNumber);
    }

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

    void setAppID(QString appID)
    {
        if (m_appID == appID)
            return;

        m_appID = appID;
        emit appIDChanged(m_appID);
    }

private:
    QVector<int> m_screenNumber;
    QString m_projectPath;
    QString m_previewImage;
    QProcess* m_process;
    QSharedPointer<ProjectSettingsListModel> m_projectSettingsListModel;

    QString m_appID;
};

class ScreenPlayWidget : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString projectPath READ projectPath WRITE setProjectPath NOTIFY projectPathChanged)
    Q_PROPERTY(QString fullPath READ fullPath WRITE setFullPath NOTIFY fullPathChanged)
    Q_PROPERTY(QString previewImage READ previewImage WRITE setPreviewImage NOTIFY previewImageChanged)
    Q_PROPERTY(QPoint position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)

public:
    ScreenPlayWidget(QString projectPath, QString previewImage, QString fullPath, ScreenPlay* parent)
    {
        m_projectPath = projectPath;
        m_fullPath = fullPath;
        m_previewImage = previewImage;
        m_process = new QProcess(this);

        QStringList proArgs;
        proArgs.append(m_projectPath);
        m_appID = parent->generateID();
        proArgs.append(m_appID);
        m_process->setArguments(proArgs);


        if (fullPath.endsWith(".exe")) {
            m_process->setProgram(fullPath);
        } else if (fullPath.endsWith(".qml")) {
            m_process->setProgram(parent->m_settings->getScreenPlayWidgetPath().toString());
        }
        qDebug() << m_process->program();
        m_process->start();
    }

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

    QString fullPath() const
    {
        return m_fullPath;
    }

    QString appID() const
    {
        return m_appID;
    }

signals:

    void projectPathChanged(QString projectPath);

    void previewImageChanged(QString previewImage);

    void positionChanged(QPoint position);

    void fullPathChanged(QString fullPath);

    void appIDChanged(QString appID);

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

    void setFullPath(QString fullPath)
    {
        if (m_fullPath == fullPath)
            return;

        m_fullPath = fullPath;
        emit fullPathChanged(m_fullPath);
    }

    void setAppID(QString appID)
    {
        if (m_appID == appID)
            return;

        m_appID = appID;
        emit appIDChanged(m_appID);
    }

private:
    QString m_projectPath;
    QString m_previewImage;
    QPoint m_position;
    QProcess* m_process;
    QString m_fullPath;
    QString m_appID;
};
