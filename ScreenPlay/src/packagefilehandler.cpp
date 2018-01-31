#include "packagefilehandler.h"

PackageFileHandler::PackageFileHandler(QObject* parent)
    : QObject(parent)
{
    qRegisterMetaType<PackageFileHandler::LoaderStatus>("PackageFileHandler::LoaderStatus");
    qmlRegisterType<PackageFileHandler>("PackageFileHandler", 1, 0, "LoaderStatus");
}

int PackageFileHandler::loadPackageFromLocalZip(QVector<QUrl> url, QString extractDir)
{
    setCurrentLoaderStatus(LoaderStatus::Extracting);
    for(int i = 0; i < url.length(); i++) {
        if(!QString(url.at(i).toString()).endsWith(".zip")){
            return LoaderStatus::SomeErrors;
        }
    }
    for(int i = 0; i < url.length(); i++){
        //QStringList list = JlCompress::extractDir(url.at(i).toString(), extractDir);

    }




    return LoaderStatus::Successful;
}
