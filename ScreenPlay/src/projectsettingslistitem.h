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

namespace ScreenPlay {

/*!
    \class ScreenPlay::ProjectSettingsListItem
    \inmodule ScreenPlay
    \brief List Item for Project Settings List Model. See ProjectSettingsListModel for more informations.


*/

struct IListItem {
    //MOC complains otherwise WTF
    bool operator!=(const IListItem& val)
    {
        return true;
    }
};

struct SliderItem : public IListItem {
    Q_GADGET
    Q_PROPERTY(double from MEMBER m_from)
    Q_PROPERTY(double to MEMBER m_to)
    Q_PROPERTY(double stepSize MEMBER m_stepSize)
    Q_PROPERTY(double value MEMBER m_value)
public:
    double m_from = { 0.0 };
    double m_to = { 0.1 };
    double m_stepSize = { 0.1 };
    double m_value = { 0.0 };
};

struct ColorItem : public IListItem {
    Q_GADGET
    Q_PROPERTY(QColor color MEMBER m_color)
public:
    QColor m_color = { QColor::fromRgb(0, 0, 0) };
};
struct CheckBoxItem : public IListItem {
    Q_GADGET
    Q_PROPERTY(bool value MEMBER m_value)
public:
    bool m_value = { false };
};
struct FileItem : public IListItem {
    Q_GADGET
    Q_PROPERTY(QUrl file MEMBER m_file)
public:
    QUrl m_file = {};
};

struct ProjectSettingsListItem {
    Q_GADGET
    Q_PROPERTY(IListItem item MEMBER m_item)
public:
    ProjectSettingsListItem(
        const QString& headline)
    {
        m_isHeadline = true;
        m_key = headline;
    }

    ProjectSettingsListItem(
        const QString& key,
        const QJsonObject& obj)
    {
        m_isHeadline = false;
        m_key = key;
        QString type = obj.value("type").toString();
        qInfo() << type;
        if (type == "slider") {
            SliderItem item;
            item.m_from = obj.value("from").toDouble(0);
            item.m_to = obj.value("to").toDouble(1);
            item.m_stepSize = obj.value("stepSize").toDouble(0.1);
            item.m_value = obj.value("value").toDouble(1);
            m_item = item;
            return;
        }

        if (type == "file") {
            FileItem item;
            item.m_file = obj.value("file").toString();
            m_item = item;
            return;
        }

        if (type == "color") {
            ColorItem item;
            item.m_color = { obj.value("file").toString() };
            m_item = item;
            return;
        }

        if (type == "bool") {
            CheckBoxItem item;

            m_item = item;
            return;
        }
    }

    QString m_key;
    bool m_isHeadline { false };
    QVariant m_value;
    IListItem m_item;
    QVariant value() const
    {
        return m_value;
    }

};


}
