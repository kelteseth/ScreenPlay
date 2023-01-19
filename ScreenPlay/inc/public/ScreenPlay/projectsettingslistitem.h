// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QColor>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QUrl>
#include <QVariant>

#include <memory>

namespace ScreenPlay {

/*!
    \class ScreenPlay::ProjectSettingsListItem
    \inmodule ScreenPlay
    \brief List Item for Project Settings List Model. See ProjectSettingsListModel for more informations.


*/

struct IListItem {
    Q_GADGET
public:
    QVariant m_value;
    virtual QJsonObject getItemSettings() = 0;
public slots:
    virtual void setData(const QJsonObject& obj) { }
};

struct SliderItem : public IListItem {
    Q_GADGET

    Q_PROPERTY(double from MEMBER m_from)
    Q_PROPERTY(double to MEMBER m_to)
    Q_PROPERTY(double stepSize MEMBER m_stepSize)
    Q_PROPERTY(double value MEMBER m_value)
public:
    double m_from = { 0.0 };
    double m_value = { 0.0 };
    double m_to = { 0.1 };
    double m_stepSize = { 0.1 };
    QJsonObject getItemSettings() override;
public slots:
    void setData(const QJsonObject& obj) override;
};

struct ColorItem : public IListItem {
    Q_GADGET
    Q_PROPERTY(QColor color MEMBER m_color)
public:
    QJsonObject getItemSettings() override;
    QColor m_color = { QColor::fromRgb(0, 0, 0) };
public slots:
    void setData(const QJsonObject& obj) override;
};

struct CheckBoxItem : public IListItem {
    Q_GADGET
    Q_PROPERTY(bool checked MEMBER m_checked)
public:
    QJsonObject getItemSettings() override;
    bool m_checked = { false };
public slots:
    void setData(const QJsonObject& obj) override;
};

struct FileItem : public IListItem {
    Q_GADGET
    Q_PROPERTY(QUrl file MEMBER m_file)
    Q_PROPERTY(QString formats MEMBER m_formats)
public:
    QJsonObject getItemSettings() override;
    QUrl m_file = {};
    QString m_formats = {};
public slots:
    void setData(const QJsonObject& obj) override;
};

struct ProjectSettingsListItem {
    Q_GADGET
public:
    ProjectSettingsListItem(
        const QString& headline);

    ProjectSettingsListItem(
        const QString& key,
        const QJsonObject& obj);

    QString m_key;
    bool m_isHeadline { false };
    std::shared_ptr<IListItem> m_item;
};

}
