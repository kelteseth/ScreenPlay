// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include "ScreenPlay/screenplayexternalprocess.h"
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
        const QPoint& position,
        const QString& absolutePath,
        const QString& previewImage, const QJsonObject& properties,
        const ContentTypes::InstalledType type);

    bool start() override;
    QCoro::Task<Result> close() override;

    QPoint position() const { return m_position; }
    QJsonObject getActiveSettingsJson();

signals:
    void positionChanged(QPoint position);
    void requestSave();

public slots:
    void setPosition(QPoint position);

protected:
    void setupSDKConnection() override;

private:
    QPoint m_position;
};
}
