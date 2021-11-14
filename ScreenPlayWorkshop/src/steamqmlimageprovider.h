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
    ~SteamQMLImageProvider()
    {
        m_texture->deleteLater();
    }
    QSGNode* updatePaintNode(QSGNode* oldNode, QQuickItem::UpdatePaintNodeData*)
    {
        QSGSimpleTextureNode* node = static_cast<QSGSimpleTextureNode*>(oldNode);
        if (!node) {
            node = new QSGSimpleTextureNode();
        }
        m_texture = window()->createTextureFromImage(m_image);
        node->setTexture(m_texture);
        node->setRect(boundingRect());
        return node;
    }
public slots:
    void setImage(QImage image)
    {

        m_image = image.scaledToWidth(boundingRect().width(), Qt::TransformationMode::SmoothTransformation);

        update();
    }

private:
    QImage m_image;
    QSGTexture* m_texture;
};
}
