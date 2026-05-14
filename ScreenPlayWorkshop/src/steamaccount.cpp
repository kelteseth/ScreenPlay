// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "steamaccount.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(workshopAccount, "screenplay.workshop.account")

namespace ScreenPlayWorkshop {

/*!
    \class ScreenPlayWorkshop::SteamAccount
    \inmodule ScreenPlayWorkshop
    \brief Exposes Steam user identity and avatar to QML.

    Wraps Steam user data — username, account ID, steamID64, large profile
    avatar, and subscribed item count — as Qt properties so they can be
    consumed directly from QML.
*/

/*!
    \fn SteamAccount::SteamAccount(QObject *parent)
    \brief Constructs a SteamAccount with the given \a parent and immediately
           populates username, account ID, and subscribed item count from the
           Steam API.
*/
SteamAccount::SteamAccount(QObject* parent)
    : QObject(parent)
{
    m_steamID = SteamUser()->GetSteamID();
    setUsername(SteamFriends()->GetPersonaName());
    setAccountID(m_steamID.GetAccountID());
    setAmountSubscribedItems(SteamUGC()->GetNumSubscribedItems());
}

/*!
    \fn void SteamAccount::loadAvatar()
    \brief Requests and caches the user's large (184 px) Steam avatar.

    If the avatar is already cached the \c avatarChanged signal is emitted
    immediately. If \c GetLargeFriendAvatar returns \c -1 the image is not
    yet available; the method returns early and waits for the
    \c AvatarImageLoaded_t callback handled by \c onAvatarImageLoaded().
*/
void SteamAccount::loadAvatar()
{
    if (!m_avatar.isNull()) {
        // Avatar already loaded, emit signal for any new listeners
        emit avatarChanged(m_avatar);
        return;
    }
    int largeFriendAvatarHandle = SteamFriends()->GetLargeFriendAvatar(m_steamID);

    // Returns 0 if no avatar is set for the user.
    // Returns -1 if the avatar image data has not been loaded yet and requests that it gets download.
    // In this case wait for a AvatarImageLoaded_t callback and then call this again.
    // see SteamAccount::onAvatarImageLoaded
    if (largeFriendAvatarHandle <= 0) {
        qCWarning(workshopAccount) << "onAvatarImageLoaded: getLargeFriendAvatarResult retunred: " << largeFriendAvatarHandle;
        return;
    }

    AvatarImageLoaded_t avatarImage;
    avatarImage.m_steamID = m_steamID;
    avatarImage.m_iImage = largeFriendAvatarHandle;
    // Large -> 184px
    avatarImage.m_iWide = 184;
    avatarImage.m_iTall = 184;

    // We can call the callback ourself if directly available
    onAvatarImageLoaded(&avatarImage);
}

/*!
    \fn void SteamAccount::loadAmountSubscribedItems()
    \brief Refreshes \c amountSubscribedItems from the Steam UGC API.
*/
void SteamAccount::loadAmountSubscribedItems()
{
    setAmountSubscribedItems(SteamUGC()->GetNumSubscribedItems());
}

/*!
    \fn void SteamAccount::onAvatarImageLoaded(AvatarImageLoaded_t *avatarImage)
    \brief Internal Steam callback invoked when the avatar image data for
           \a avatarImage becomes available. Reads the RGBA pixel data via
           \c SteamUtils()->GetImageRGBA, constructs a \c QImage, and emits
           \c avatarChanged. Subsequent calls are no-ops once the avatar has
           been loaded.
*/
void SteamAccount::onAvatarImageLoaded(AvatarImageLoaded_t* avatarImage)
{
    if (m_avatarLoaded)
        return;

    const int largeFriendAvatarHandle = SteamFriends()->GetLargeFriendAvatar(m_steamID);

    if (largeFriendAvatarHandle <= 0) {
        qCWarning(workshopAccount) << "onAvatarImageLoaded: GetLargeFriendAvatar returned: " << largeFriendAvatarHandle;
        return;
    }

    uint32 width = 0;
    uint32 height = 0;
    const bool sizeRetrieved = SteamUtils()->GetImageSize(avatarImage->m_iImage, &width, &height);
    if (!sizeRetrieved) {
        qCWarning(workshopAccount) << "onAvatarImageLoaded: Failed to get image size";
        return;
    }

    const int imageSize = width * height * 4;
    QVector<uint8> imageData(imageSize);
    const bool imageRetrieved = SteamUtils()->GetImageRGBA(avatarImage->m_iImage, imageData.data(), imageSize);
    if (!imageRetrieved) {
        qCWarning(workshopAccount) << "onAvatarImageLoaded: Failed to load image buffer from callback";
        return;
    }

    // QImage constructor with data pointer doesn't copy - must call copy() to own the data
    const QImage avatar = QImage { imageData.data(), static_cast<int>(width), static_cast<int>(height), QImage::Format_RGBA8888 }.copy();
    setAvatar(avatar);
    m_avatarLoaded = true;
}
}

#include "moc_steamaccount.cpp"
