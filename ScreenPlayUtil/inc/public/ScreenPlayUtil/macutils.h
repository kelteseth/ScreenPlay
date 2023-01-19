// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QObject>

class MacUtils : public QObject {
    Q_OBJECT

private:
    static MacUtils* macUtils;
    MacUtils();

public:
    static MacUtils* instance();
    static void showDockIcon(const bool show);
};
