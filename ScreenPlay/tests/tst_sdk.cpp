// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

// Unit tests for the ScreenPlay SDK IPC layer.
//
// Test coverage:
//  - SDKConnection::readyRead() — ping frame, appID handshake, command=requestRaise,
//    JSON object, mixed frames in one packet (regression for the return→continue fix).
//  - SDKConnection::sendMessage() — round-trip echo via QLocalServer.
//  - SDKConnection::close() — returns true on clean close (regression for async fix).
//
// All tests use a QLocalServer backed with a random name so they are fully in-process
// with no real wallpaper/widget processes involved.

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocalServer>
#include <QLocalSocket>
#include <QSignalSpy>
#include <QTest>
#include <memory>

#include "ScreenPlay/sdkconnection.h"

using namespace ScreenPlay;

// ---------------------------------------------------------------------------
// Helper: create a connected (client, SDKConnection) pair via a temporary
// QLocalServer.  The server socket is owned by *serverSocket_out.
// ---------------------------------------------------------------------------
struct Pair {
    std::unique_ptr<QLocalServer> server;
    std::unique_ptr<QLocalSocket> serverClientSock; // raw server-side socket
    std::unique_ptr<SDKConnection> conn; // wraps serverClientSock
    std::unique_ptr<QLocalSocket> client; // writer side (represents the wallpaper)
};

static Pair makePair()
{
    Pair p;
    p.server = std::make_unique<QLocalServer>();
    p.server->setSocketOptions(QLocalServer::WorldAccessOption);
    // Use a unique name per call to avoid cross-test collisions
    const QString name = "tst_sdk_" + QString::number(QCoreApplication::applicationPid())
        + "_" + QString::number(reinterpret_cast<quintptr>(&p));
    p.server->listen(name);

    p.client = std::make_unique<QLocalSocket>();
    p.client->connectToServer(name);
    p.client->waitForConnected(1000);

    p.server->waitForNewConnection(1000);
    QLocalSocket* rawSock = p.server->nextPendingConnection();
    p.serverClientSock.reset(rawSock);
    p.conn = std::make_unique<SDKConnection>(rawSock);

    return p;
}

// Write from the client side and process events so the server readyRead fires
static void clientWrite(Pair& p, const QByteArray& data)
{
    p.client->write(data);
    p.client->waitForBytesWritten(500);
    QTest::qWait(50); // spin event loop so readyRead is dispatched
}

// ---------------------------------------------------------------------------
// Test class
// ---------------------------------------------------------------------------
class TstSDK : public QObject {
    Q_OBJECT

private slots:
    // ------------------------------------------------------------------
    // SDKConnection::readyRead tests
    // ------------------------------------------------------------------

    void pingEmitsPingAliveReceived()
    {
        auto p = makePair();
        QSignalSpy spy(p.conn.get(), &SDKConnection::pingAliveReceived);
        clientWrite(p, "ping;");
        QCOMPARE(spy.count(), 1);
    }

    void pingDoesNotEmitJsonMessage()
    {
        auto p = makePair();
        QSignalSpy jsonSpy(p.conn.get(), &SDKConnection::jsonMessageReceived);
        clientWrite(p, "ping;");
        QCOMPARE(jsonSpy.count(), 0);
    }

    // Regression for "return instead of continue" fix: when ping and a JSON
    // object arrive in the same TCP/socket packet the JSON must still be parsed.
    void pingPlusJsonInOnePacketBothSignalsFire()
    {
        auto p = makePair();
        QSignalSpy pingSpy(p.conn.get(), &SDKConnection::pingAliveReceived);
        QSignalSpy jsonSpy(p.conn.get(), &SDKConnection::jsonMessageReceived);

        QJsonObject obj;
        obj["key"] = "value";
        const QByteArray json = QJsonDocument(obj).toJson(QJsonDocument::Compact);
        clientWrite(p, "ping;" + json + ";");

        QCOMPARE(pingSpy.count(), 1);
        QCOMPARE(jsonSpy.count(), 1);
        QCOMPARE(jsonSpy.at(0).at(0).toJsonObject().value("key").toString(), QString("value"));
    }

    void appIDHandshakeParsedCorrectly()
    {
        auto p = makePair();
        QSignalSpy connSpy(p.conn.get(), &SDKConnection::appConnected);

        // "QMLWallpaper" is one of the known types returned by Util::getAvailableTypes()
        clientWrite(p, "appID=test-id-1234,QMLWallpaper;");

        QCOMPARE(connSpy.count(), 1);
        QCOMPARE(p.conn->appID(), QString("test-id-1234"));
        QCOMPARE(p.conn->type(), QString("QMLWallpaper"));
    }

    void appIDHandshakeUnknownTypeDoesNotCrash()
    {
        auto p = makePair();
        // An unknown type should not crash; appConnected is still emitted
        // (the connection is accepted even without a known type, just logged).
        clientWrite(p, "appID=abc,UnknownTypeXYZ;");
        // No crash — test passes if we reach this point
        QVERIFY(true);
    }

    // Regression for dead-code requestRaise fix
    void commandRequestRaiseEmitsSignal()
    {
        auto p = makePair();
        QSignalSpy spy(p.conn.get(), &SDKConnection::requestRaise);
        clientWrite(p, "command=requestRaise;");
        QCOMPARE(spy.count(), 1);
    }

    void validJsonObjectEmitsJsonMessageReceived()
    {
        auto p = makePair();
        QSignalSpy spy(p.conn.get(), &SDKConnection::jsonMessageReceived);

        QJsonObject obj;
        obj["command"] = "replace";
        obj["volume"] = 0.5;
        const QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Compact) + ";";

        clientWrite(p, data);

        QCOMPARE(spy.count(), 1);
        QJsonObject received = spy.at(0).at(0).toJsonObject();
        QCOMPARE(received.value("command").toString(), QString("replace"));
    }

    void malformedJsonDoesNotCrashAndNoSignalEmitted()
    {
        auto p = makePair();
        QSignalSpy spy(p.conn.get(), &SDKConnection::jsonMessageReceived);
        clientWrite(p, "{this is not json};");
        // No crash and no signal
        QCOMPARE(spy.count(), 0);
    }

    void multiplePingsInOnePacketEachEmitsSignal()
    {
        auto p = makePair();
        QSignalSpy spy(p.conn.get(), &SDKConnection::pingAliveReceived);
        clientWrite(p, "ping;ping;ping;");
        QCOMPARE(spy.count(), 3);
    }

    // ------------------------------------------------------------------
    // SDKConnection::sendMessage tests
    // ------------------------------------------------------------------

    void sendMessageDeliversBytesToClient()
    {
        auto p = makePair();
        QSignalSpy readySpy(p.client.get(), &QLocalSocket::readyRead);

        const QByteArray msg = "hello;";
        bool ok = p.conn->sendMessage(msg);
        QVERIFY(ok);

        if (readySpy.isEmpty())
            QVERIFY(readySpy.wait(1000));

        QCOMPARE(p.client->readAll(), msg);
    }

    // ------------------------------------------------------------------
    // SDKConnection::close() tests
    // ------------------------------------------------------------------

    // Regression for the close()-returns-false (async disconnect) fix.
    void closeReturnsTrueOnCleanDisconnect()
    {
        auto p = makePair();

        // The client must be alive and connected for a clean close.
        QCOMPARE(p.client->state(), QLocalSocket::ConnectedState);

        bool result = p.conn->close();
        QVERIFY(result);
    }

    void closeOnAlreadyClosedSocketReturnsFalseOrDoesNotCrash()
    {
        auto p = makePair();
        p.conn->close();
        // Second close: must not crash.  Return value may be false.
        p.conn->close();
        QVERIFY(true);
    }
};

QTEST_MAIN(TstSDK)
#include "tst_sdk.moc"
