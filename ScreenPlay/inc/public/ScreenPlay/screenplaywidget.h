// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include "ScreenPlay/screenplayexternalprocess.h"
#include "ScreenPlay/widgetdata.h"
#include <QPoint>

namespace ScreenPlay {

class ScreenPlayWidget : public ScreenPlayExternalProcess {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    Q_PROPERTY(QPoint position READ position WRITE setPosition NOTIFY positionChanged FINAL)
public:
    explicit ScreenPlayWidget(
        const QString& appID,
        const std::shared_ptr<GlobalVariables>& globalVariables,
        const WidgetData& widgetData,
        QObject* parent = nullptr);

    bool start() override;
    QCoro::Task<Result> close() override;

    QPoint position() const { return m_widgetData.position(); }
    QJsonObject getActiveSettingsJson();

signals:
    void positionChanged(QPoint position);
    void requestSave();

public slots:
    void setPosition(QPoint position);

protected:
    void setupSDKConnection() override;

private:
    WidgetData m_widgetData;
};
}
