#include "sdkconnection.h"

namespace ScreenPlay {

/*!
    \class ScreenPlay::SDKConnection
    \inmodule ScreenPlay
    \brief  Contains all connections to Wallpaper and Widgets.


*/

/*!
    \brief Constructor.
*/
ScreenPlay::SDKConnection::SDKConnection(QLocalSocket* socket, QObject* parent)
    : QObject(parent)
{

    m_socket = socket;
    connect(m_socket, &QLocalSocket::readyRead, this, &SDKConnection::readyRead);
}

/*!
    \brief Read incomming messages. Checks for types like:
    \list 1
        \li ping: Used to check if wallpaper is still alive
        \li appID: First message of an app must contain the ID to match it to our list of running apps
        \li command: Used mainly for requestRaise. This will get fired if the user tries to open a second ScreenPlay instance
        \li general Json object
    \endlist
*/
void ScreenPlay::SDKConnection::readyRead()
{

    auto msg = QString(m_socket->readAll());

    if (msg == "ping") {
        emit pingAliveReceived();
        return;
    }

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

        qInfo() << "New connection" << m_appID << msg;

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

/*!
    \brief Sends a message to the connected socket.
*/
void ScreenPlay::SDKConnection::sendMessage(const QByteArray& message)
{
    m_socket->write(message);
    m_socket->waitForBytesWritten();
}

/*!
    \brief Closes the socket connection. Before it explicitly sends a quit command to make sure
           the wallpaper closes (fast). This also requestDecreaseWidgetCount() because Widgets.
*/
void ScreenPlay::SDKConnection::close()
{

    qInfo() << "Close " << m_type << m_appID;

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
}
