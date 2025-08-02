// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QAbstractListModel>
#include <QDateTime>
#include <QObject>
#include <QQmlEngine>
#include <QQueue>
#include <QTimer>

namespace ScreenPlay {

/*!
    \brief Represents a single error entry with timestamp and message
*/
struct ErrorEntry {
    Q_GADGET
    Q_PROPERTY(QString message MEMBER message CONSTANT)
    Q_PROPERTY(QDateTime timestamp MEMBER timestamp CONSTANT)
    Q_PROPERTY(QString formattedTimestamp READ formattedTimestamp CONSTANT)

public:
    QString message;
    QDateTime timestamp;

    QString formattedTimestamp() const
    {
        return timestamp.toString("hh:mm:ss");
    }

    ErrorEntry() = default;
    ErrorEntry(const QString& msg)
        : message(msg)
        , timestamp(QDateTime::currentDateTime())
    {
    }
};

/*!
    \class ScreenPlay::ErrorManager
    \inmodule ScreenPlay
    \brief Manages error messages and displays them to the user.

    The ErrorManager queues errors that occur before QML is ready and provides
    a model for displaying them in a QML popup. It prevents spamming the user
    with multiple popups by collecting errors and showing them in a single dialog.

    The ErrorManager also handles critical errors from external processes such as
    wallpapers and widgets that fail to restart after multiple attempts.
*/
class ErrorManager : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Use App.errorManager instead")

    Q_PROPERTY(bool hasErrors READ hasErrors NOTIFY hasErrorsChanged)
    Q_PROPERTY(int errorCount READ errorCount NOTIFY errorCountChanged)

public:
    enum Roles {
        MessageRole = Qt::UserRole + 1,
        TimestampRole,
        FormattedTimestampRole
    };

    explicit ErrorManager(QObject* parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    bool hasErrors() const { return !m_errors.isEmpty(); }
    int errorCount() const { return m_errors.size(); }

public slots:
    /*!
        \brief Adds an error message to the queue
        If QML is ready, it will immediately show the popup.
        Otherwise, it will queue the error for later display.
    */
    void displayError(const QString& message);

    /*!
        \brief Clears all error messages
    */
    void clearErrors();

    /*!
        \brief Marks QML as ready to display error popups
    */
    void setQmlReady(bool ready);

signals:
    void hasErrorsChanged();
    void errorCountChanged();
    void showErrorPopup();

private slots:
    void processQueuedErrors();

private:
    QList<ErrorEntry> m_errors;
    QQueue<QString> m_queuedErrors;
    bool m_qmlReady = false;
    QTimer m_queueTimer;

    static constexpr int MAX_ERRORS = 50; // Limit stored errors to prevent memory issues

    void addError(const QString& message);
};
}

Q_DECLARE_METATYPE(ScreenPlay::ErrorEntry)
