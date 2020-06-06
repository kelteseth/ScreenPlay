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

#include <QString>
#include <QVariant>

namespace ScreenPlay {

/*!
    \class ScreenPlay::ProjectSettingsListItem
    \inmodule ScreenPlay
    \brief List Item for Project Settings List Model. See ProjectSettingsListModel for more informations.


*/

struct ProjectSettingsListItem {

    ProjectSettingsListItem(
        const QString& name,
        const bool isHeadline,
        const QVariant& value)
    {
        m_name = name;
        m_isHeadline = isHeadline;
        m_value = value;
    }

    QString m_name;
    bool m_isHeadline { false };
    QVariant m_value;
};
}
