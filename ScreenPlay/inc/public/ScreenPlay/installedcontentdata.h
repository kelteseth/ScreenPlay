// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include "ScreenPlayCore/contenttypes.h"
#include <QJsonObject>
#include <QObject>
#include <QString>

namespace ScreenPlay {

/*!
    \brief Base class for installed content data (wallpapers and widgets).
    
    This class contains all the shared properties that both wallpapers and widgets
    have in common, such as title, absolutePath, previewImage, type, file, and properties.
    This enables better error reporting using user-facing titles instead of internal appIDs.
*/
class InstalledContentData {
    Q_GADGET
    QML_ANONYMOUS
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString absolutePath READ absolutePath WRITE setAbsolutePath)
    Q_PROPERTY(QString previewImage READ previewImage WRITE setPreviewImage)
    Q_PROPERTY(QString file READ file WRITE setFile)
    Q_PROPERTY(QJsonObject properties READ properties WRITE setProperties)
    Q_PROPERTY(ScreenPlay::ContentTypes::InstalledType type READ type WRITE setType)

public:
    // Getters
    QString title() const { return m_title; }
    QString absolutePath() const { return m_absolutePath; }
    QString previewImage() const { return m_previewImage; }
    QString file() const { return m_file; }
    QJsonObject properties() const { return m_properties; }
    ContentTypes::InstalledType type() const { return m_type; }

    // Setters
    void setTitle(const QString& title) { m_title = title; }
    void setAbsolutePath(const QString& absolutePath) { m_absolutePath = absolutePath; }
    void setPreviewImage(const QString& previewImage) { m_previewImage = previewImage; }
    void setFile(const QString& file) { m_file = file; }
    void setProperties(const QJsonObject& properties) { m_properties = properties; }
    void setType(ContentTypes::InstalledType type) { m_type = type; }

    Q_INVOKABLE bool hasContent() const;
    Q_INVOKABLE QString toString() const;

protected:
    // Helper method for serialization of common properties
    QJsonObject serializeBase() const;
    
    // Helper method for loading common properties from JSON
    void loadBaseFromJson(const QJsonObject& obj);

private:
    QString m_title;
    QString m_absolutePath;
    QString m_previewImage;
    QString m_file;
    QJsonObject m_properties;
    ContentTypes::InstalledType m_type = ContentTypes::InstalledType::Unknown;
};

}
