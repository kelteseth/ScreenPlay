// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/errormanager.h"

namespace ScreenPlay {

ErrorManager::ErrorManager(QObject* parent)
    : QAbstractListModel(parent)
{
    // Timer to batch process queued errors
    m_queueTimer.setSingleShot(true);
    m_queueTimer.setInterval(100); // Small delay to batch multiple errors
    connect(&m_queueTimer, &QTimer::timeout, this, &ErrorManager::processQueuedErrors);
}

int ErrorManager::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return m_errors.size();
}

QVariant ErrorManager::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_errors.size())
        return QVariant();

    const ErrorEntry& error = m_errors.at(index.row());

    switch (role) {
    case MessageRole:
        return error.message;
    case TimestampRole:
        return error.timestamp;
    case FormattedTimestampRole:
        return error.formattedTimestamp();
    }

    return QVariant();
}

QHash<int, QByteArray> ErrorManager::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[MessageRole] = "message";
    roles[TimestampRole] = "timestamp";
    roles[FormattedTimestampRole] = "formattedTimestamp";
    return roles;
}

void ErrorManager::displayError(const QString& message)
{
    if (message.isEmpty())
        return;

    if (m_qmlReady) {
        addError(message);
        emit showErrorPopup();
    } else {
        // Queue the error for later processing when QML is ready
        m_queuedErrors.enqueue(message);
        if (!m_queueTimer.isActive()) {
            m_queueTimer.start();
        }
    }
}

void ErrorManager::clearErrors()
{
    if (m_errors.isEmpty())
        return;

    beginResetModel();
    m_errors.clear();
    endResetModel();

    emit hasErrorsChanged();
    emit errorCountChanged();
}

void ErrorManager::setQmlReady(bool ready)
{
    m_qmlReady = ready;
    if (ready && !m_queuedErrors.isEmpty()) {
        // Process any queued errors now that QML is ready
        processQueuedErrors();
    }
}

void ErrorManager::processQueuedErrors()
{
    if (!m_qmlReady || m_queuedErrors.isEmpty())
        return;

    // Add all queued errors to the model
    bool hasNewErrors = false;
    while (!m_queuedErrors.isEmpty()) {
        const QString message = m_queuedErrors.dequeue();
        addError(message);
        hasNewErrors = true;
    }

    if (hasNewErrors) {
        emit showErrorPopup();
    }
}

void ErrorManager::addError(const QString& message)
{
    // Prevent duplicate consecutive errors
    if (!m_errors.isEmpty() && m_errors.last().message == message) {
        return;
    }

    beginInsertRows(QModelIndex(), m_errors.size(), m_errors.size());

    m_errors.append(ErrorEntry(message));

    // Limit the number of stored errors to prevent memory issues
    if (m_errors.size() > MAX_ERRORS) {
        beginRemoveRows(QModelIndex(), 0, 0);
        m_errors.removeFirst();
        endRemoveRows();
    }

    endInsertRows();

    emit hasErrorsChanged();
    emit errorCountChanged();
}

}

#include "moc_errormanager.cpp"
