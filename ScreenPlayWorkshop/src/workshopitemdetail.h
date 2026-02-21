// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QtQml/qqmlregistration.h>

namespace ScreenPlayWorkshop {

/*!
    \brief Lightweight value type carrying the result of a single-item
           workshop detail query (used by Sidebar.qml).

    Declared as Q_GADGET so every member is accessible from QML without
    needing a full QObject.
*/
struct WorkshopItemDetail {
    Q_GADGET
    QML_VALUE_TYPE(workshopItemDetail)

    Q_PROPERTY(QString title       MEMBER title)
    Q_PROPERTY(QStringList tags    MEMBER tags)
    Q_PROPERTY(qulonglong steamIDOwner MEMBER steamIDOwner)
    Q_PROPERTY(QString description MEMBER description)
    Q_PROPERTY(quint64 votesUp     MEMBER votesUp)
    Q_PROPERTY(quint64 votesDown   MEMBER votesDown)
    Q_PROPERTY(QString url         MEMBER url)
    Q_PROPERTY(QVariant fileSize   MEMBER fileSize)
    Q_PROPERTY(QVariant publishedFileId MEMBER publishedFileId)

public:
    QString title;
    QStringList tags;
    qulonglong steamIDOwner {};
    QString description;
    quint64 votesUp {};
    quint64 votesDown {};
    QString url;
    QVariant fileSize;
    QVariant publishedFileId;
};

/*!
    \brief Lightweight value type carrying the result of a profile-item
           workshop detail query (used by SteamProfileWorkshopItem.qml).
*/
struct WorkshopProfileItemDetail {
    Q_GADGET
    QML_VALUE_TYPE(workshopProfileItemDetail)

    Q_PROPERTY(QVariant publishedFileId   MEMBER publishedFileId)
    Q_PROPERTY(QString title              MEMBER title)
    Q_PROPERTY(QString description        MEMBER description)
    Q_PROPERTY(QStringList tags           MEMBER tags)
    Q_PROPERTY(qulonglong steamIDOwner    MEMBER steamIDOwner)
    Q_PROPERTY(quint64 votesUp            MEMBER votesUp)
    Q_PROPERTY(quint64 votesDown          MEMBER votesDown)
    Q_PROPERTY(float score                MEMBER score)
    Q_PROPERTY(QString url                MEMBER url)
    Q_PROPERTY(QVariant fileSize          MEMBER fileSize)
    Q_PROPERTY(QVariant totalFileSize     MEMBER totalFileSize)
    Q_PROPERTY(QString previewUrl         MEMBER previewUrl)
    Q_PROPERTY(quint32 timeCreated        MEMBER timeCreated)
    Q_PROPERTY(quint32 timeUpdated        MEMBER timeUpdated)
    Q_PROPERTY(int visibility             MEMBER visibility)
    Q_PROPERTY(bool banned                MEMBER banned)
    Q_PROPERTY(bool acceptedForUse        MEMBER acceptedForUse)
    Q_PROPERTY(quint64 subscriptionCount  MEMBER subscriptionCount)
    Q_PROPERTY(quint64 favoriteCount      MEMBER favoriteCount)
    Q_PROPERTY(quint64 followerCount      MEMBER followerCount)
    Q_PROPERTY(quint64 uniqueWebsiteViews MEMBER uniqueWebsiteViews)
    Q_PROPERTY(quint32 numChildren        MEMBER numChildren)

public:
    QVariant publishedFileId;
    QString title;
    QString description;
    QStringList tags;
    qulonglong steamIDOwner {};
    quint64 votesUp {};
    quint64 votesDown {};
    float score {};
    QString url;
    QVariant fileSize;
    QVariant totalFileSize;
    QString previewUrl;
    quint32 timeCreated {};
    quint32 timeUpdated {};
    int visibility {};
    bool banned {};
    bool acceptedForUse {};
    quint64 subscriptionCount {};
    quint64 favoriteCount {};
    quint64 followerCount {};
    quint64 uniqueWebsiteViews {};
    quint32 numChildren {};
};

} // namespace ScreenPlayWorkshop
