#include "screenplay.h"
#include "screenplaywallpaper.h"



ScreenPlayWallpaper::ScreenPlayWallpaper(const std::vector<int>& screenNumber, const QString& projectPath,
    const QString& previewImage, const float volume,
    const QString& fillMode, const QString& type, ScreenPlay* parent)
    : QObject { parent }
    , m_screenNumber { std::move(screenNumber) }
    , m_projectPath { projectPath }
    , m_previewImage { previewImage }
    , m_type { type }
    , m_appID { parent ? parent->generateID() : QString {} }
    , m_process { this }
    , m_projectSettingsListModel { QSharedPointer<ProjectSettingsListModel>::create(projectPath + "/project.json") }
{
    const QStringList proArgs {
       QString::number(m_screenNumber.empty() ? 0 : m_screenNumber[0]),
       m_projectPath,
       QString{"appID=" + m_appID},
       parent ? parent->settings()->decoder() : QString {},
       QString::number(static_cast<double>(volume)),
       fillMode
    };

    qDebug() << "creating ScreenPlayWallpaper...\n" << proArgs;

    // We do not want to parent the QProcess because the
    // Process manages its lifetime and destructing (animation) itself
    // via a disconnection from the ScreenPlay SDK
    // QProcess m_process = new QProcess();

    QObject::connect(&m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [=](int exitCode, QProcess::ExitStatus exitStatus) {
            if (exitCode != 0)
                qDebug() << "WARNING EXIT CODE: " << exitCode;
        });
    QObject::connect(&m_process, &QProcess::errorOccurred, [=](QProcess::ProcessError error) {
        qDebug() << "EX: " << error;
    });

    m_process.setArguments(proArgs);
    m_process.setProgram(parent->settings()->screenPlayWindowPath().toString());
    m_process.start();
}

ScreenPlayWallpaper::~ScreenPlayWallpaper()
{
    //if (m_process) delete m_process;
}

const std::vector<int>& ScreenPlayWallpaper::screenNumber() const noexcept
{
    return m_screenNumber;
}

const QString& ScreenPlayWallpaper::projectPath() const noexcept
{
    return m_projectPath;
}

const QString& ScreenPlayWallpaper::previewImage() const noexcept
{
    return m_previewImage;
}

const QString& ScreenPlayWallpaper::type() const noexcept
{
    return m_type;
}

const QString& ScreenPlayWallpaper::appID() const noexcept
{
    return m_appID;
}

const QSharedPointer<ProjectSettingsListModel>& ScreenPlayWallpaper::projectSettingsListModel() const noexcept
{
    return m_projectSettingsListModel;
}

void ScreenPlayWallpaper::setScreenNumber(const std::vector<int>& screenNumber) noexcept
{
    if (m_screenNumber == screenNumber)
        return;
    m_screenNumber = screenNumber;
    emit screenNumberChanged(m_screenNumber);
}

void ScreenPlayWallpaper::setProjectPath(const QString& projectPath) noexcept
{
    if (m_projectPath == projectPath)
        return;

    m_projectPath = projectPath;
    emit projectPathChanged(m_projectPath);
}

void ScreenPlayWallpaper::setPreviewImage(const QString& previewImage) noexcept
{
    if (m_previewImage == previewImage)
        return;

    m_previewImage = previewImage;
    emit previewImageChanged(m_previewImage);
}

void ScreenPlayWallpaper::setType(const QString& type) noexcept
{
    if (m_type == type)
        return;

    m_type = type;
    emit typeChanged(m_type);
}

void ScreenPlayWallpaper::setAppID(const QString& appID) noexcept
{
    if (m_appID == appID)
        return;

    m_appID = appID;
    emit appIDChanged(m_appID);
}
