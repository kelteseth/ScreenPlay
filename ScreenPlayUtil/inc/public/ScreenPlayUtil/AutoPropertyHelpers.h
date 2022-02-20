#pragma once

#include <QObject>

#include "HelpersCommon.h"

#define AUTO_GETTER(type, name) \
public:                         \
    CheapestType<type>::type_def MAKE_GETTER_NAME(name)() const { return m_##name; }

#define AUTO_SETTER(type, name)                        \
public:                                                \
    void set_##name(CheapestType<type>::type_def name) \
    {                                                  \
        if (m_##name != name) {                        \
            m_##name = name;                           \
            Q_EMIT name##Changed();                    \
        }                                              \
    }

#define W_AUTO_PROPERTY(type, name)                                                         \
protected:                                                                                  \
    Q_PROPERTY(type name READ MAKE_GETTER_NAME(name) WRITE set_##name NOTIFY name##Changed) \
                                                                                            \
    MEMBER(type, name)                                                                      \
    AUTO_GETTER(type, name)                                                                 \
    AUTO_SETTER(type, name)                                                                 \
    NOTIFIER(name)                                                                          \
private:

#define R_AUTO_PROPERTY(type, name)                                        \
protected:                                                                 \
    Q_PROPERTY(type name READ MAKE_GETTER_NAME(name) NOTIFY name##Changed) \
                                                                           \
    MEMBER(type, name)                                                     \
    AUTO_GETTER(type, name)                                                \
    AUTO_SETTER(type, name)                                                \
    NOTIFIER(name)                                                         \
private:

#define C_AUTO_PROPERTY(type, name)                            \
protected:                                                     \
    Q_PROPERTY(type name READ MAKE_GETTER_NAME(name) CONSTANT) \
                                                               \
    MEMBER(type, name)                                         \
    AUTO_GETTER(type, name)                                    \
private:

#define W_AUTO_PROPERTY_DEFAULT(type, name, defaultValue)                                   \
    Q_PROPERTY(type name READ MAKE_GETTER_NAME(name) WRITE set_##name NOTIFY name##Changed) \
                                                                                            \
    MEMBER_DEFAULT(type, name, defaultValue)                                                \
    AUTO_GETTER(type, name)                                                                 \
    AUTO_SETTER(type, name)                                                                 \
    NOTIFIER(name)                                                                          \
private:

// NOTE : test class for all cases

class _Test_QmlAutoProperty_ : public QObject {
    Q_OBJECT

    W_AUTO_PROPERTY(bool, var1)
    W_AUTO_PROPERTY(QString, var2)
    W_AUTO_PROPERTY(QObject*, var3)

    R_AUTO_PROPERTY(bool, var4)
    R_AUTO_PROPERTY(QString, var5)
    R_AUTO_PROPERTY(QObject*, var6)

    C_AUTO_PROPERTY(bool, var7)
    C_AUTO_PROPERTY(QString, var8)
    C_AUTO_PROPERTY(QObject*, var9)

    W_AUTO_PROPERTY_DEFAULT(bool, var10, true)
};
