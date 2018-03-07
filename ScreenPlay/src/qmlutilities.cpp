#include "qmlutilities.h"

#include <QProcess>

QMLUtilities::QMLUtilities(QObject *parent) : QObject(parent)
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
    explorer.setProgram("explorer.exe");
    QStringList args;
    const int folderLength = 2000;
    char folder[folderLength];
    SteamApps()->GetAppInstallDir(672870, folder, static_cast<uint32>(folderLength));
    QByteArray folderData(folder);
    args.append(QDir::toNativeSeparators(QString(folderData + "/ScreenPlay/legal")));
    explorer.setArguments(args);
    explorer.startDetached();
}
