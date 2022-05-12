#pragma once

#include <QJSEngine>
#include <QObject>
#include <QQmlEngine>
#include <qqml.h>

#define QML_SINGLETON_IMPL(CLASS)                                                                                    \
public:                                                                                                              \
    static CLASS& instance()                                                                                         \
    {                                                                                                                \
        static CLASS ret;                                                                                            \
        return ret;                                                                                                  \
    }                                                                                                                \
    static QObject* qmlSingletonFactory(QQmlEngine* qmlEngine, QJSEngine* jsEngine)                                  \
    {                                                                                                                \
        Q_UNUSED(jsEngine)                                                                                           \
        Q_UNUSED(qmlEngine)                                                                                          \
        QObject* ret = &instance();                                                                                  \
        QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);                                               \
        return ret;                                                                                                  \
    }                                                                                                                \
    static void registerQmlModule(const char* uri, const int majorVersion, const int minorVersion, const char* name) \
    {                                                                                                                \
        qmlRegisterSingletonType<CLASS>(uri, majorVersion, minorVersion, name, &CLASS::qmlSingletonFactory);         \
    }

class _test_QmlSingleton_ : public QObject {
    Q_OBJECT
    QML_SINGLETON_IMPL(_test_QmlSingleton_)
};
