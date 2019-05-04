#pragma once

#include <QObject>
#include <QPoint>
#include <QProcess>

/*!
    \class ScreenPlayWidget
    \brief Used for ...
*/
class ScreenPlay;

class ScreenPlayWidget final : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString projectPath READ projectPath WRITE setProjectPath NOTIFY projectPathChanged)
    Q_PROPERTY(QString fullPath READ fullPath WRITE setFullPath NOTIFY fullPathChanged)
    Q_PROPERTY(QString previewImage READ previewImage WRITE setPreviewImage NOTIFY previewImageChanged)
    Q_PROPERTY(QPoint position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)

private:
    QString m_projectPath;
    QString m_previewImage;
    QString m_fullPath;
    QString m_appID;
    QPoint m_position;
    QProcess m_process;

public:
    // constructor(s)
    explicit ScreenPlayWidget(const QString& projectPath, const QString& previewImage,
        const QString& fullPath, ScreenPlay* parent = nullptr);

    // copy and move disable(for now) : remember rule of 1/3/5
    //Q_DISABLE_COPY_MOVE(ScreenPlayWidget)
    Q_DISABLE_COPY(ScreenPlayWidget)
    ScreenPlayWidget(ScreenPlayWidget&&) = delete;
    ScreenPlayWidget& operator=(ScreenPlayWidget&&) = delete;

    // destructor
    ~ScreenPlayWidget();

    // getters
    const QString& projectPath() const noexcept;
    const QString& previewImage() const noexcept;
    const QString& fullPath() const noexcept;
    const QString& appID() const noexcept;
    const QPoint& position() const noexcept;

signals:
    void projectPathChanged(QString projectPath) const;
    void previewImageChanged(QString previewImage) const;
    void fullPathChanged(QString fullPath) const;
    void appIDChanged(QString appID) const;
    void positionChanged(QPoint position) const;

public slots:
    void setProjectPath(const QString& projectPath) noexcept;
    void setPreviewImage(const QString& previewImage) noexcept;
    void setFullPath(const QString& fullPath) noexcept;
    void setAppID(const QString& appID) noexcept;
    void setPosition(const QPoint& position) noexcept;
};
