// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once
#include <QWindow>

class MacBridge {

private:
    static MacBridge* bridge;
    MacBridge();

public:
    static MacBridge* instance();
    static void SetBackgroundLevel(QWindow* window);
};
