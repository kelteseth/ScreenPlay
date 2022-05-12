#pragma once

#include <QDebug>
#include <QImage>
#include <QObject>
#include <QPixmap>
#include <QString>
#include <QTimer>
#include <QtGlobal>
#include <QtQml>

#include <memory>
#include <steam/steam_api.h>

namespace ScreenPlayWorkshop {

class SteamAccount : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(quint32 accountID READ accountID WRITE setAccountID NOTIFY accountIDChanged)
    Q_PROPERTY(QImage avatar READ avatar WRITE setAvatar NOTIFY avatarChanged)
    Q_PROPERTY(quint32 amountSubscribedItems READ amountSubscribedItems WRITE setAmountSubscribedItems NOTIFY amountSubscribedItemsChanged)

public:
    explicit SteamAccount(QObject* parent = nullptr);

    QString username() const { return m_username; }
    quint32 accountID() const { return m_accountID; }
    QImage avatar() const { return m_avatar; }
    quint32 amountSubscribedItems() const { return m_amountSubscribedItems; }

public slots:
    void loadAvatar();
    void loadAmountSubscribedItems();
    void setUsername(QString username)
    {
        if (m_username == username)
            return;

        m_username = username;
        emit usernameChanged(m_username);
    }

    void setAccountID(quint32 accountID)
    {
        if (m_accountID == accountID)
            return;

        m_accountID = accountID;
        emit accountIDChanged(m_accountID);
    }

    void setAvatar(QImage avatar)
    {
        if (m_avatar == avatar)
            return;

        m_avatar = avatar;
        emit avatarChanged(m_avatar);
    }

    void setAmountSubscribedItems(quint32 amountSubscribedItems)
    {
        if (m_amountSubscribedItems == amountSubscribedItems)
            return;

        m_amountSubscribedItems = amountSubscribedItems;
        emit amountSubscribedItemsChanged(m_amountSubscribedItems);
    }

signals:
    void usernameChanged(QString username);
    void accountIDChanged(quint32 accountID);
    void avatarChanged(QImage avatar);
    void amountSubscribedItemsChanged(quint32 amountSubscribedItems);

private:
    STEAM_CALLBACK(SteamAccount, onAvatarImageLoaded, AvatarImageLoaded_t);

private:
    QString m_username;
    quint32 m_accountID {};
    quint32 m_amountSubscribedItems {};
    QImage m_avatar;
    CSteamID m_steamID;
    bool m_avatarLoaded = false;
};
}
