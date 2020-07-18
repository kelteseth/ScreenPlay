#include "sdkconnection.h"

ScreenPlay::SDKConnection::SDKConnection(QLocalSocket* socket, QObject* parent)
    : QObject(parent)
{

    m_socket = socket;
    connect(m_socket, &QLocalSocket::readyRead, this, &SDKConnection::readyRead);
}

ScreenPlay::SDKConnection::~SDKConnection()
{

    // We need to call this manually because
    // sometimes it wont close the connection in
    // the descructor
    m_socket->disconnect();
    m_socket->disconnectFromServer();
}

void ScreenPlay::SDKConnection::readyRead()
{

    auto msg = QString(m_socket->readAll());

    // The first message allways contains the appID
    if (msg.startsWith("appID=")) {
        QStringList args = msg.split(",");
        //Only use the first 32 chars for the appID
        QString appID = args.at(0);
        m_appID = appID.remove("appID=");

        bool typeFound = false;
        for (const QString& type : GlobalVariables::getAvailableTypes()) {
            if (msg.contains(type, Qt::CaseInsensitive)) {
                m_type = type;
                typeFound = true;
                break;
            }
        }

        if (!typeFound) {
            qCritical() << "Wallpaper type not found. Expected: " << GlobalVariables::getAvailableTypes() << " got: " << msg;
        }

        emit appConnected(this);

    } else if (msg.startsWith("command=")) {
        msg.remove("command=");
        if (msg == "requestRaise") {
            qInfo() << "Another ScreenPlay instance reuqested this one to raise!";
            emit requestRaise();
        }
    } else if (msg.startsWith("{") && msg.endsWith("}")) {
        QJsonObject obj;
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray { msg.toUtf8() }, &err);

        if (err.error != QJsonParseError::NoError)
            return;

        emit jsonMessageReceived(doc.object());

    } else {
        qInfo() << "### Message from: " << m_appID << ": " << msg;
    }
}

void ScreenPlay::SDKConnection::close()
{

    qInfo() << "Close " << m_type;

    QJsonObject obj;
    obj.insert("command", QJsonValue("quit"));
    QByteArray command = QJsonDocument(obj).toJson();

    m_socket->write(command);
    m_socket->waitForBytesWritten();

    if (m_socket->state() == QLocalSocket::ConnectedState) {
        m_socket->disconnectFromServer();
        m_socket->close();

        qDebug() << "### Destroy APPID:\t " << m_appID << " State: " << m_socket->state();
    }

    if (m_type.contains("widget", Qt::CaseInsensitive)) {
        emit requestDecreaseWidgetCount();
    }
}
