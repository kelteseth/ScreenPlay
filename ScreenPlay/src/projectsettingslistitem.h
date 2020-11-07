/****************************************************************************
**
** Copyright (C) 2020 Elias Steurer (Kelteseth)
** Contact: https://screen-play.app
**
** This file is part of ScreenPlay. ScreenPlay is licensed under a dual license in
** order to ensure its sustainability. When you contribute to ScreenPlay
** you accept that your work will be available under the two following licenses:
**
** $SCREENPLAY_BEGIN_LICENSE$
**
** #### Affero General Public License Usage (AGPLv3)
** Alternatively, this file may be used under the terms of the GNU Affero
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file "ScreenPlay License.md" included in the
** packaging of this App. Please review the following information to
** ensure the GNU Affero Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/agpl-3.0.en.html.
**
** #### Commercial License
** This code is owned by Elias Steurer. By changing/adding to the code you agree to the
** terms written in:
**  * Legal/corporate_contributor_license_agreement.md - For corporate contributors
**  * Legal/individual_contributor_license_agreement.md - For individual contributors
**
** #### Additional Limitations to the AGPLv3 and Commercial Lincese
** This License does not grant any rights in the trademarks,
** service marks, or logos.
**
**
** $SCREENPLAY_END_LICENSE$
**
****************************************************************************/

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
