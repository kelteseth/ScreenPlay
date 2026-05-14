// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once
#include <QImage>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSGNode>
#include <QSGSimpleTextureNode>
#include <QSGTexture>
#include <QtQml>

namespace ScreenPlayWorkshop {
class SteamQMLImageProvider : public QQuickItem {
    Q_OBJECT
    QML_NAMED_ELEMENT(SteamImage)

public:
    SteamQMLImageProvider(QQuickItem* parent);
    SteamQMLImageProvider() { setFlag(QQuickItem::ItemHasContents); }

    QSGNode* updatePaintNode(QSGNode* oldNode, QQuickItem::UpdatePaintNodeData*) override
    {
        // Don't render anything until we have an image
        if (m_image.isNull() || !window()) {
            delete oldNode;
            return nullptr;
        }

        QSGSimpleTextureNode* node = static_cast<QSGSimpleTextureNode*>(oldNode);
        if (!node) {
            node = new QSGSimpleTextureNode();
            // Let the node own and delete the texture on the render thread
            node->setOwnsTexture(true);
        }

        // Only create new texture if image changed
        if (m_imageChanged) {
            // Node owns the texture, so setting a new one will delete the old one
            node->setTexture(window()->createTextureFromImage(m_image));
            m_imageChanged = false;
        }

        node->setRect(boundingRect());
        return node;
    }
public slots:
    void setImage(QImage image)
    {
        if (image.isNull() || boundingRect().width() <= 0)
            return;

        m_image = image.scaledToWidth(boundingRect().width(), Qt::TransformationMode::SmoothTransformation);
        m_imageChanged = true;
        update();
    }

private:
    QImage m_image;
    bool m_imageChanged = false;
};
}
