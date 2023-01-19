// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QString>
#include <QUrl>
#include <QVariant>
#include <QtDebug>

/*!
    \class Steam Workshop Item
    \brief Used by the Steam Workshop List Model Class

*/

namespace ScreenPlayWorkshop {

struct WorkshopItem {
    WorkshopItem(
        const QVariant publishedFileID,
        const quint64 subscriptionCount,
        const QString& title,
        const QUrl& previewImageUrl,
        const QUrl& additionalPreviewUrl)
    {
        m_publishedFileID = publishedFileID;
        m_subscriptionCount = subscriptionCount;
        m_title = title;
        m_previewImageUrl = previewImageUrl;
        m_additionalPreviewUrl = additionalPreviewUrl;
    }
    QUrl m_previewImageUrl = {};
    QString m_title = {};
    QUrl m_additionalPreviewUrl = {};
    quint64 m_subscriptionCount = {};
    QVariant m_publishedFileID = {};
};
}
