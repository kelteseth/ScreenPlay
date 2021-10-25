#include "httpfileserver.h"
#include "httplib.h"

namespace ScreenPlay {
HttpFileServer::HttpFileServer(QObject* parent)
    : QObject { parent }
{
}

void HttpFileServer::startServer()
{
    const auto deleter = [](auto* ptr) { delete ptr; };
    m_server = std::unique_ptr<httplib::Server, Deleter>(new httplib::Server(), deleter);
    m_thread = std::thread([&]() {
        qInfo() << " set_mount_point" << m_server->set_mount_point("/", "C:/Users/Eli/Desktop/web");

        qInfo() << "listen";
        m_server->listen("0.0.0.0", 8081);
        qInfo() << "end";
    });
}

void HttpFileServer::stopServer()
{
    if (m_thread.joinable())
        m_thread.join();
}
}
