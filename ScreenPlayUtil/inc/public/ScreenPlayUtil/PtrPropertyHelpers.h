#pragma once

#include <QObject>
#include <memory>

#include "HelpersCommon.h"

#define PTR_GETTER(type, name)           \
public:                                  \
    type* MAKE_GETTER_NAME(name)() const \
    {                                    \
        return m_##name;                 \
    }

#define PTR_SETTER(type, name)      \
public:                             \
    void set_##name(type* name)     \
    {                               \
        if (m_##name != name) {     \
            m_##name = name;        \
            Q_EMIT name##Changed(); \
        }                           \
    }

#define SMART_PTR_GETTER(type, name)     \
public:                                  \
    type* MAKE_GETTER_NAME(name)() const \
    {                                    \
        return m_##name.get();           \
    }

#define SMART_PTR_SETTER(type, name) \
public:                              \
    void set_##name(type name)       \
    {                                \
        m_##name = std::move(name);  \
        Q_EMIT name##Changed();      \
    }

#define W_PTR_PROPERTY(type, name)                                                           \
protected:                                                                                   \
    Q_PROPERTY(type* name READ MAKE_GETTER_NAME(name) WRITE set_##name NOTIFY name##Changed) \
                                                                                             \
    MEMBER(type*, name)                                                                      \
    PTR_GETTER(type, name)                                                                   \
    PTR_SETTER(type, name)                                                                   \
    NOTIFIER(name)                                                                           \
private:

#define R_PTR_PROPERTY(type, name)                                          \
protected:                                                                  \
    Q_PROPERTY(type* name READ MAKE_GETTER_NAME(name) NOTIFY name##Changed) \
                                                                            \
    MEMBER(type*, name)                                                     \
    PTR_GETTER(type, name)                                                  \
    PTR_SETTER(type, name)                                                  \
    NOTIFIER(name)                                                          \
private:

#define C_PTR_PROPERTY(type, name)                              \
protected:                                                      \
    Q_PROPERTY(type* name READ MAKE_GETTER_NAME(name) CONSTANT) \
                                                                \
    MEMBER(type*, name)                                         \
    PTR_GETTER(type, name)                                      \
private:

#define W_PTR_PROPERTY_DEFAULT(type, name, defaultValue)                                     \
protected:                                                                                   \
    Q_PROPERTY(type* name READ MAKE_GETTER_NAME(name) WRITE set_##name NOTIFY name##Changed) \
                                                                                             \
    MEMBER_DEFAULT(type*, name, defaultValue)                                                \
    PTR_GETTER(type, name)                                                                   \
    PTR_SETTER(type, name)                                                                   \
    NOTIFIER(name)                                                                           \
private:

#define UNIQUE_PTR_PROPERTY(type, name)                   \
protected:                                                \
    Q_PROPERTY(type* name READ name NOTIFY name##Changed) \
                                                          \
    MEMBER(std::unique_ptr<type>, name)                   \
    SMART_PTR_GETTER(type, name)                          \
    SMART_PTR_SETTER(std::unique_ptr<type>&, name)        \
    NOTIFIER(name)                                        \
private:

#define UNIQUE_PTR_PROPERTY_INIT(type, name)                              \
protected:                                                                \
    Q_PROPERTY(type* name READ name NOTIFY name##Changed)                 \
                                                                          \
    MEMBER_DEFAULT(std::unique_ptr<type>, name, std::make_unique<type>()) \
    SMART_PTR_GETTER(type, name)                                          \
    SMART_PTR_SETTER(std::unique_ptr<type>&, name)                        \
    NOTIFIER(name)                                                        \
private:

#define SHARED_PTR_PROPERTY(type, name)                   \
protected:                                                \
    Q_PROPERTY(type* name READ name NOTIFY name##Changed) \
                                                          \
    MEMBER(std::shared_ptr<type>, name)                   \
    SMART_PTR_GETTER(type, name)                          \
    SMART_PTR_SETTER(std::unique_ptr<type>&, name)        \
    NOTIFIER(name)                                        \
private:

#define SHARED_PTR_PROPERTY_INIT(type, name)                              \
protected:                                                                \
    Q_PROPERTY(type* name READ name NOTIFY name##Changed)                 \
                                                                          \
    MEMBER_DEFAULT(std::shared_ptr<type>, name, std::make_shared<type>()) \
    SMART_PTR_GETTER(type, name)                                          \
    SMART_PTR_SETTER(std::shared_ptr<type>, name)                         \
    NOTIFIER(name)                                                        \
private:

class _QmlPtrProperty_ : public QObject {
    Q_OBJECT

    W_PTR_PROPERTY(int, var1)
    R_PTR_PROPERTY(bool, var2)
    C_PTR_PROPERTY(QString, test)

    //  W_PTR_PROPERTY_DEFAULT(QString, var4, nullptr)

    //  UNIQUE_PTR_PROPERTY(QString, test2)
    //  UNIQUE_PTR_PROPERTY_INIT(QString, test3)

    //  SHARED_PTR_PROPERTY(QString, test4)
    //  SHARED_PTR_PROPERTY_INIT(QString, test5)
};
