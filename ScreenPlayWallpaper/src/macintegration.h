// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QApplication>

class MacIntegration : public QObject {
    Q_OBJECT
public:
    explicit MacIntegration(QObject* parent);
    void SetBackgroundLevel(QWindow* window);
};
