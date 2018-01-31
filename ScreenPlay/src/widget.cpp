#include "widget.h"

Widget::Widget(QWindow *parent) : QWindow(parent)
{
    m_quickView = new QQuickView(this);
    m_quickView->setSource(QUrl("qrc:/qml/Components/Widgets/Window.qml"));
    m_quickView->show();
}
