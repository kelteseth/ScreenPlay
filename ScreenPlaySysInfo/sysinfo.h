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

#include <QQuickItem>
#include <memory>

#include "cpu.h"
#include "ram.h"
#include "storage.h"
#include "uptime.h"

class SysInfo : public QQuickItem {
    Q_OBJECT

    Q_PROPERTY(RAM* ram READ ram NOTIFY ramChanged)
    Q_PROPERTY(CPU* cpu READ cpu NOTIFY cpuChanged)
    Q_PROPERTY(Storage* storage READ storage NOTIFY storageChanged)
    Q_PROPERTY(Uptime* uptime READ uptime NOTIFY uptimeChanged)

public:
    SysInfo(QQuickItem* parent = nullptr);

    RAM* ram() const { return m_ram.get(); }
    CPU* cpu() const { return m_cpu.get(); }
    Storage* storage() const { return m_storage.get(); }
    Uptime* uptime() const { return m_uptime.get(); }

signals:
    void ramChanged(RAM* ram);
    void cpuChanged(CPU* cpu);
    void storageChanged(Storage* storage);
    void uptimeChanged(Uptime* uptime);

private:
    std::unique_ptr<RAM> m_ram;
    std::unique_ptr<CPU> m_cpu;
    std::unique_ptr<Storage> m_storage;
    std::unique_ptr<Uptime> m_uptime;
};
