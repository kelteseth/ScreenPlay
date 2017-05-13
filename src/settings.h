#ifndef SCREENPLAYSETTINGS_H
#define SCREENPLAYSETTINGS_H

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject >
#include <QQmlPropertyMap>
#include <QStandardPaths>
#include <QString>
#include <QVariant>
#include <QDebug>

class Settings : public QObject {
    Q_OBJECT
public:
    explicit Settings(QObject* parent = nullptr);

    Q_PROPERTY(QVariant version READ version)
    Q_PROPERTY(bool autostart READ autostart WRITE setAutostart NOTIFY autostartChanged)
    Q_PROPERTY(bool highPriorityStart READ highPriorityStart WRITE setHighPriorityStart NOTIFY highPriorityStartChanged)
    Q_PROPERTY(Renderer renderer READ renderer WRITE setRenderer NOTIFY rendererChanged)
    Q_PROPERTY(bool sendStatistics READ sendStatistics WRITE setSendStatistics NOTIFY sendStatisticsChanged)

    enum Renderer {
        OpenGL,
        Softare,
    };
    Q_ENUM(Renderer)

    QVariant version() const
    {
        return m_version;
    }

    bool autostart() const
    {
        return m_autostart;
    }

    bool highPriorityStart() const
    {
        return m_highPriorityStart;
    }

    Renderer renderer() const
    {
        return m_renderer;
    }

    bool sendStatistics() const
    {
        return m_sendStatistics;
    }

signals:

    void autostartChanged(bool autostart);

    void highPriorityStartChanged(bool highPriorityStart);

    void rendererChanged(Renderer renderer);

    void sendStatisticsChanged(bool sendStatistics);

public slots:

    void setAutostart(bool autostart)
    {
        qDebug() << autostart;
        if (m_autostart == autostart)
            return;

        m_autostart = autostart;
        emit autostartChanged(m_autostart);
    }

    void setHighPriorityStart(bool highPriorityStart)
    {
        if (m_highPriorityStart == highPriorityStart)
            return;

        m_highPriorityStart = highPriorityStart;
        emit highPriorityStartChanged(m_highPriorityStart);
    }

    void setRenderer(Renderer renderer)
    {
        if (m_renderer == renderer)
            return;

        m_renderer = renderer;
        emit rendererChanged(m_renderer);
    }

    void setsendStatistics(bool sendStatistics)
    {
        if (m_sendStatistics == sendStatistics)
            return;

        m_sendStatistics = sendStatistics;
        emit sendStatisticsChanged(m_sendStatistics);
    }

    void setSendStatistics(bool sendStatistics)
    {
        if (m_sendStatistics == sendStatistics)
            return;

        m_sendStatistics = sendStatistics;
        emit sendStatisticsChanged(m_sendStatistics);
    }

private:
    void createDefaultConfig();

    QVariant m_version = "";
    bool m_autostart = true;
    bool m_highPriorityStart = true;
    Renderer m_renderer = Renderer::OpenGL;
    bool m_sendStatistics;
};

#endif // SCREENPLAYSETTINGS_H
