#ifndef WALLPAPER_H
#define WALLPAPER_H

#include <QDebug>
#include <QQmlContext>
#include <QQuickView>
#include <QRect>
#include <QWindow>
#include <qt_windows.h>
#include <QUrl>
#include <QDir>

#include "profile.h"
#include "projectfile.h"

class Wallpaper : public QWindow {
    Q_OBJECT
public:
    explicit Wallpaper(QWindow* parent = 0);
    Wallpaper(Profile profile, ProjectFile projf);
    Q_PROPERTY(QString absoluteFilePath READ absoluteFilePath WRITE setAbsoluteFilePath NOTIFY absoluteFilePathChanged)




    QString absoluteFilePath() const
    {
        return m_absoluteFilePath;
    }

public slots:
    void setAbsoluteFilePath(QString absoluteFilePath)
    {
        if (m_absoluteFilePath == absoluteFilePath)
            return;

        m_absoluteFilePath = absoluteFilePath;
        emit absoluteFilePathChanged(m_absoluteFilePath);
    }

signals:
    void absoluteFilePathChanged(QString absoluteFilePath);

private:
    HWND m_hwnd;
    HWND m_worker_hwnd;
    QQuickView* m_quickRenderer = nullptr;
    QQmlContext* m_context = nullptr;

    Profile m_profile;
    ProjectFile m_project;
    QString m_absoluteFilePath;
};

#endif // WALLPAPER_H
