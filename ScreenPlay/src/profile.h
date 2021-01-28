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

#include <QRect>
#include <QString>
#include <QUrl>

namespace ScreenPlay {

/*!
    \class ScreenPlay::Profile
    \inmodule ScreenPlay
    \brief  A classed used in combination with Profile List Model.
            See Profile List Model for more details

*/
struct Profile {

    Profile() { }
    Profile(const QUrl& absolutePath,
        const QString& id,
        const QString& version,
        const QString& wallpaperId,
        const QRect& rect,
        const bool isLooping)
    {
        m_absolutePath = absolutePath;
        m_id = id;
        m_version = version;
        m_wallpaperId = wallpaperId;
        m_rect = rect;
        m_isLooping = isLooping;
    }

    QUrl m_absolutePath;
    QString m_id;
    QString m_version = "";
    QString m_wallpaperId = "";
    QRect m_rect;
    bool m_isLooping = true;
    QVector<QString> m_monitorList;
};
}
