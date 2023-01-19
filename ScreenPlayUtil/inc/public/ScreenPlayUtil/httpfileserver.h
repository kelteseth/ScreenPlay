// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QDebug>
#include <QObject>

#include <memory>
#include <thread>

namespace httplib {
class Server;
}

namespace ScreenPlay {

class HttpFileServer : public QObject {
    Q_OBJECT
public:
    explicit HttpFileServer(QObject* parent = nullptr);

    void startServer();
    void stopServer();

private:
    using Deleter = std::function<void(httplib::Server*)>;
    std::unique_ptr<httplib::Server, Deleter> m_server;

    std::thread m_thread;
};
}
