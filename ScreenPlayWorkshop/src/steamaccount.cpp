// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "steamaccount.h"

namespace ScreenPlayWorkshop {

SteamAccount::SteamAccount(QObject* parent)
    : QObject(parent)
{
    m_steamID = SteamUser()->GetSteamID();
    setUsername(SteamFriends()->GetPersonaName());
    setAccountID(m_steamID.GetAccountID());
    setAmountSubscribedItems(SteamUGC()->GetNumSubscribedItems());
}

void SteamAccount::loadAvatar()
{
    if (!m_avatar.isNull()) {
        return;
    }
    int largeFriendAvatarHandle = SteamFriends()->GetLargeFriendAvatar(m_steamID);

    // Returns 0 if no avatar is set for the user.
    // Returns -1 if the avatar image data has not been loaded yet and requests that it gets download.
    // In this case wait for a AvatarImageLoaded_t callback and then call this again.
    // see SteamAccount::onAvatarImageLoaded
    if (largeFriendAvatarHandle <= 0) {
        qWarning() << "onAvatarImageLoaded: getLargeFriendAvatarResult retunred: " << largeFriendAvatarHandle;
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

void SteamAccount::loadAmountSubscribedItems()
{
    setAmountSubscribedItems(SteamUGC()->GetNumSubscribedItems());
}

void SteamAccount::onAvatarImageLoaded(AvatarImageLoaded_t* avatarImage)
{
    if (m_avatarLoaded)
        return;

    const int largeFriendAvatarHandle = SteamFriends()->GetLargeFriendAvatar(m_steamID);

    if (largeFriendAvatarHandle <= 0) {
        qWarning() << "onAvatarImageLoaded: GetLargeFriendAvatar returned: " << largeFriendAvatarHandle;
        return;
    }

    uint32 width = 0;
    uint32 height = 0;
    const bool sizeRetrieved = SteamUtils()->GetImageSize(avatarImage->m_iImage, &width, &height);
    if (!sizeRetrieved) {
        qWarning() << "onAvatarImageLoaded: Failed to get image size";
        return;
    }

    const int imageSize = width * height * 4;
    QVector<uint8> imageData(imageSize);
    const bool imageRetrieved = SteamUtils()->GetImageRGBA(avatarImage->m_iImage, imageData.data(), imageSize);
    if (!imageRetrieved) {
        qWarning() << "onAvatarImageLoaded: Failed to load image buffer from callback";
        return;
    }

    const QImage avatar { imageData.data(), static_cast<int>(width), static_cast<int>(height), QImage::Format_RGBA8888 };
    setAvatar(avatar);
    m_avatarLoaded = true;
}
}

#include "moc_steamaccount.cpp"
