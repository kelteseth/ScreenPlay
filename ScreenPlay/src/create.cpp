#include "create.h"


Create::Create(Settings *st, QMLUtilities *util, QObject *parent) : QObject(parent)
{
    m_settings = st;
    m_utils = util;
}

void Create::copyProject(QString relativeProjectPath, QString toPath)
{
    if(toPath.contains("file:///")){
        toPath.remove("file:///");
    }
    QString srcFilePath = m_settings->getScreenPlayBasePath().toString() + relativeProjectPath ;

    // Todo: UI Error handling
    if(copyRecursively(srcFilePath, toPath)){
        emit m_utils->openFolderInExplorer(toPath);
    }
}

bool Create::copyRecursively(const QString &srcFilePath, const QString &tgtFilePath)
{
    QFileInfo srcFileInfo(srcFilePath);
       if (srcFileInfo.isDir()) {
           QDir targetDir(tgtFilePath);
           targetDir.cdUp();
           QDir sourceDir(srcFilePath);
           QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
           foreach (const QString &fileName, fileNames) {
               const QString newSrcFilePath
                       = srcFilePath + QLatin1Char('/') + fileName;
               const QString newTgtFilePath
                       = tgtFilePath + QLatin1Char('/') + fileName;
               if (!copyRecursively(newSrcFilePath, newTgtFilePath))
                   return false;
           }
       } else {
           if (!QFile::copy(srcFilePath, tgtFilePath))
               return false;
       }
   return true;
}
