// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once



class MacBridge : public QObject {
    Q_OBJECT

private:
    static MacBridge* bridge;
    MacBridge();

public:
    static MacBridge* instance();
    static void SetBackgroundLevel(QWindow* window);
};
