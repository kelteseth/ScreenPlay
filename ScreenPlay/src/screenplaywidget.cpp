#include "screenplay.h"
#include "screenplaywidget.h"



ScreenPlayWidget::ScreenPlayWidget(const QString& projectPath, const QString& previewImage,
    const QString& fullPath, ScreenPlay* parent)
    : QObject{ parent }
    , m_projectPath { projectPath }
    , m_previewImage { previewImage }
    , m_fullPath { fullPath }
    , m_appID { parent ? parent->generateID() : QString {} }
    , m_position { 0, 0 }
    , m_process { this } //PLS LESS BEHINDERT @Elias
{
    const QStringList proArgs{ m_projectPath, m_appID};
    m_process.setArguments(proArgs);

    if (fullPath.endsWith(".exe")) {
        m_process.setProgram(fullPath);
    } else if (fullPath.endsWith(".qml")) {
        m_process.setProgram(parent->settings()->getScreenPlayWidgetPath().path());
    }

    qDebug() << m_process.program();

    QObject::connect(&m_process, &QProcess::errorOccurred, this, [](QProcess::ProcessError error) {
        qDebug() << "error: " << error;
    });
    m_process.start();
}

ScreenPlayWidget::~ScreenPlayWidget()
{
    //if (m_process)         delete m_process;
}

const QString& ScreenPlayWidget::projectPath() const noexcept
{
    return m_projectPath;
}

const QString& ScreenPlayWidget::previewImage() const noexcept
{
    return m_previewImage;
}

const QString& ScreenPlayWidget::fullPath() const noexcept
{
    return m_fullPath;
}

const QString& ScreenPlayWidget::appID() const noexcept
{
    return m_appID;
}

const QPoint& ScreenPlayWidget::position() const noexcept
{
    return m_position;
}

void ScreenPlayWidget::setProjectPath(const QString& projectPath) noexcept
{
    if (m_projectPath == projectPath)
        return;

    m_projectPath = projectPath;
    emit projectPathChanged(m_projectPath);
}

void ScreenPlayWidget::setPreviewImage(const QString& previewImage) noexcept
{
    if (m_previewImage == previewImage)
        return;

    m_previewImage = previewImage;
    emit previewImageChanged(m_previewImage);
}

void ScreenPlayWidget::setFullPath(const QString& fullPath) noexcept
{
    if (m_fullPath == fullPath)
        return;

    m_fullPath = fullPath;
    emit fullPathChanged(m_fullPath);
}

void ScreenPlayWidget::setAppID(const QString& appID) noexcept
{
    if (m_appID == appID)
        return;

    m_appID = appID;
    emit appIDChanged(m_appID);
}

void ScreenPlayWidget::setPosition(const QPoint& position) noexcept
{
    if (m_position == position)
        return;

    m_position = position;
    emit positionChanged(m_position);
}
