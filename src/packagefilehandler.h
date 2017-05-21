#ifndef PACKAGEFILEHANDLER_H
#define PACKAGEFILEHANDLER_H

#include <QFile>
#include <QList>
#include <QObject>
#include <QString>
#include <QUrl>
#include <QtQml>

class PackageFileHandler : public QObject {
    Q_OBJECT

    Q_PROPERTY(LoaderStatus currentLoaderStatus READ currentLoaderStatus WRITE setCurrentLoaderStatus NOTIFY currentLoaderStatusChanged)

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

    LoaderStatus currentLoaderStatus() const
    {
        return m_currentLoaderStatus;
    }

public slots:
    void setCurrentLoaderStatus(LoaderStatus currentLoaderStatus)
    {
        if (m_currentLoaderStatus == currentLoaderStatus)
            return;

        m_currentLoaderStatus = currentLoaderStatus;
        emit currentLoaderStatusChanged(currentLoaderStatus);
    }

signals:
    void currentLoaderStatusChanged(LoaderStatus currentLoaderStatus);

private:

    LoaderStatus m_currentLoaderStatus = LoaderStatus::Idle;
};

#endif // PACKAGEFILEHANDLER_H
