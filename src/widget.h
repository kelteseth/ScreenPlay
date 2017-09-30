#ifndef WIDGET_H
#define WIDGET_H

#include <QQuickItem>
#include <QQuickView>
#include <QWindow>
#include <QUrl>

class Widget : public QWindow {
    Q_OBJECT
public:
    explicit Widget(QWindow* parent = nullptr);

signals:

public slots:

private:
    QQuickItem* m_quickItem;
    QQuickView* m_quickView;
    QUrl m_source;
};

#endif // WIDGET_H
