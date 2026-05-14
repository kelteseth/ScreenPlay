// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QQuickItem>
#include <QQuickWindow>
#include <QTest>

namespace ScreenPlay {

/*!
 * \brief Finds a delegate item in a ListView/GridView by objectName.
 *
 * For some reason a direct findChild does not work for item delegates.
 * https://stackoverflow.com/questions/36767512/how-to-access-qml-listview-delegate-items-from-c
 *
 * \param listView The ListView or GridView to search in.
 * \param objectName The objectName of the delegate to find.
 * \return The found QQuickItem or nullptr if not found.
 */
inline QQuickItem* findItemDelegate(QQuickItem* listView, const QString& objectName)
{
    if (!listView || !listView->property("contentItem").isValid()) {
        return nullptr;
    }

    auto* contentItem = listView->property("contentItem").value<QQuickItem*>();
    if (!contentItem) {
        return nullptr;
    }

    const auto contentItemChildren = contentItem->childItems();
    for (auto* childItem : contentItemChildren) {
        if (childItem->objectName() == objectName) {
            return childItem;
        }
    }
    return nullptr;
}

/*!
 * \brief Simulates a mouse click on a QQuickItem.
 *
 * \param item The item to click.
 * \param button The mouse button to use (default: Qt::LeftButton).
 */
inline void clickItem(QQuickItem* item, Qt::MouseButton button = Qt::LeftButton)
{
    if (!item) {
        qWarning() << "clickItem: item is null";
        return;
    }

    QQuickWindow* itemWindow = item->window();
    if (!itemWindow) {
        qWarning() << "clickItem: item has no window";
        return;
    }

    const auto centre = item->mapToScene(QPoint(item->width() / 2, item->height() / 2)).toPoint();
    qInfo() << "clickItem:" << item->objectName() << "at" << centre;
    QTest::mouseClick(itemWindow, button, Qt::NoModifier, centre);
}

/*!
 * \brief Waits for a QML item to become visible within a timeout.
 *
 * \param item The item to wait for.
 * \param timeoutMs Maximum time to wait in milliseconds.
 * \return True if the item became visible, false on timeout.
 */
inline bool waitForItemVisible(QQuickItem* item, int timeoutMs = 5000)
{
    if (!item) {
        return false;
    }

    const int pollInterval = 100;
    int elapsed = 0;

    while (!item->isVisible() && elapsed < timeoutMs) {
        QTest::qWait(pollInterval);
        elapsed += pollInterval;
    }

    return item->isVisible();
}

/*!
 * \brief Waits for a condition to become true within a timeout.
 *
 * \param condition A callable that returns bool.
 * \param timeoutMs Maximum time to wait in milliseconds.
 * \return True if the condition became true, false on timeout.
 */
template <typename Func>
inline bool waitForCondition(Func condition, int timeoutMs = 5000)
{
    const int pollInterval = 100;
    int elapsed = 0;

    while (!condition() && elapsed < timeoutMs) {
        QTest::qWait(pollInterval);
        elapsed += pollInterval;
    }

    return condition();
}

/*!
 * \brief Finds a child item by objectName, searching recursively.
 *
 * \param parent The parent item to start searching from.
 * \param objectName The objectName to find.
 * \return The found QQuickItem or nullptr if not found.
 */
inline QQuickItem* findChildRecursive(QQuickItem* parent, const QString& objectName)
{
    if (!parent) {
        return nullptr;
    }

    // First try direct findChild
    if (auto* item = parent->findChild<QQuickItem*>(objectName)) {
        return item;
    }

    // Then search recursively through all children
    const auto children = parent->childItems();
    for (auto* child : children) {
        if (child->objectName() == objectName) {
            return child;
        }
        if (auto* found = findChildRecursive(child, objectName)) {
            return found;
        }
    }

    return nullptr;
}

} // namespace ScreenPlay
