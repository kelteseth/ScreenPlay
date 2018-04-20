#include "qmlutilities.h"

#include <QProcess>

QMLUtilities::QMLUtilities(QObject* parent)
    : QObject(parent)
{
}
void QMLUtilities::setNavigation(QString nav)
{
    emit requestNavigation(nav);
}

void QMLUtilities::setToggleWallpaperConfiguration()
{
    emit requestToggleWallpaperConfiguration();
}

QString QMLUtilities::fixWindowsPath(QString url)
{
    return url.replace("/", "\\\\");
}

void QMLUtilities::openFolderInExplorer(QString url)
{
    QProcess explorer;
    explorer.setProgram("explorer.exe");
    QStringList args;
    args.append(QDir::toNativeSeparators(url));
    explorer.setArguments(args);
    explorer.startDetached();
}

void QMLUtilities::openLicenceFolder()
{
    QProcess explorer;
    QStringList args;
    args.append(QDir::toNativeSeparators(QCoreApplication::applicationDirPath()+ "/legal"));
    explorer.setProgram("explorer.exe");
    explorer.setArguments(args);
    explorer.startDetached();
}
