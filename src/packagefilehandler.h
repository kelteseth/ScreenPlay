#ifndef PACKAGEFILEHANDLER_H
#define PACKAGEFILEHANDLER_H

#include "quazip/quazip/JlCompress.h"
#include <QFile>
#include <QList>
#include <QObject>
#include <QString>
#include <QUrl>
#include <QtQml>

class PackageFileHandler : public QObject {
    Q_OBJECT


public:
    explicit PackageFileHandler(QObject* parent = nullptr);
    Q_INVOKABLE int loadPackageFromLocalZip(QList<QUrl> url, QString extractDir);

    enum  LoaderStatus {
        Idle,
        Loading,
        Extracting,
        AllErrors,
        SomeErrors,
        Successful,
    };
    Q_ENUM(LoaderStatus)

};

#endif // PACKAGEFILEHANDLER_H
