#pragma once

#include <QObject>

#include "HelpersCommon.h"

#define CONST_SETTER(type, name)      \
public:                               \
    void set_##name(const type& name) \
    {                                 \
        if (m_##name != name) {       \
            m_##name = name;          \
            Q_EMIT name##Changed();   \
        }                             \
    }

#define CONST_GETTER(type, name) \
public:                          \
    const type& MAKE_GETTER_NAME(name)() const { return m_##name; }

#define W_CREF_PROPERTY(type, name)                                                         \
protected:                                                                                  \
    Q_PROPERTY(type name READ MAKE_GETTER_NAME(name) WRITE set_##name NOTIFY name##Changed) \
private:                                                                                    \
    MEMBER(type, name)                                                                      \
    CONST_GETTER(type, name)                                                                \
    CONST_SETTER(type, name)                                                                \
    NOTIFIER(name)

#define R_CREF_PROPERTY(type, name)                                        \
protected:                                                                 \
    Q_PROPERTY(type name READ MAKE_GETTER_NAME(name) NOTIFY name##Changed) \
    MEMBER(type, name)                                                     \
    CONST_GETTER(type, name)                                               \
    CONST_SETTER(type, name)                                               \
    NOTIFIER(name)                                                         \
private:

#define C_CREF_PROPERTY(type, name)                            \
protected:                                                     \
    Q_PROPERTY(type name READ MAKE_GETTER_NAME(name) CONSTANT) \
    MEMBER(type, name)                                         \
    CONST_GETTER(type, name)                                   \
private:

#define W_CREF_PROPERTY_DEFAULT(type, name, defaultValue)                                   \
protected:                                                                                  \
    Q_PROPERTY(type name READ MAKE_GETTER_NAME(name) WRITE set_##name NOTIFY name##Changed) \
private:                                                                                    \
    MEMBER_DEFAULT(type, name, defaultValue)                                                \
    CONST_GETTER(type, name)                                                                \
    CONST_SETTER(type, name)                                                                \
    NOTIFIER(name)

class Test : public QObject {
    Q_OBJECT

    W_CREF_PROPERTY(QString, var1)
    R_CREF_PROPERTY(QString, var2)
    C_CREF_PROPERTY(QString, var3)

    W_CREF_PROPERTY_DEFAULT(QString, var4, QStringLiteral("test"))
};
