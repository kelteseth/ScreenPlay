// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QDebug>
#include <QObject>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>
#include <QScreen>
#include <QSettings>
#include <QString>
#include <QVector>

#include "basewindow.h"
#include "macintegration.h"

namespace ScreenPlay {
class MacWindow : public BaseWindow {
    Q_OBJECT
public:
    WallpaperExit::Code start() override;

signals:

public slots:
    void setVisible(bool show) override;
    void destroyThis() override;
    void terminate() override;
    void clearComponentCache() override;

private:
};
}