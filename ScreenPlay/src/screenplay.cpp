#include "screenplay.h"

ScreenPlay::ScreenPlay(InstalledListModel* ilm, Settings* set, MonitorListModel* mlm, SDKConnector* sdkc, QObject* parent)
    : QObject{parent},
      m_ilm{ilm},
      m_settings{set},
      m_mlm{mlm},
      m_qGuiApplication{static_cast<QGuiApplication*>(QGuiApplication::instance())},
      m_sdkc{sdkc}
{}

ScreenPlay::~ScreenPlay()
{
    if(m_ilm)             delete m_ilm;
    if(m_settings)        delete m_settings;
    if(m_mlm)             delete m_mlm;
    if(m_qGuiApplication) delete m_qGuiApplication;
    if(m_sdkc)            delete m_sdkc;
}

const InstalledListModel* ScreenPlay::listModelInstalled() const noexcept { return m_ilm; }
const Settings* ScreenPlay::settings() const noexcept {   return m_settings; }
const MonitorListModel* ScreenPlay::monitorListModel() const noexcept {   return m_mlm; }
const QGuiApplication* ScreenPlay::guiApp() const noexcept {  return m_qGuiApplication; }
const SDKConnector* ScreenPlay::sdkConnector() const noexcept { return m_sdkc; }
const std::vector<RefSPWall>& ScreenPlay::spWallList() const noexcept { return m_screenPlayWallpaperList; }
const std::vector<RefSPWidget>& ScreenPlay::spWidgetList() const noexcept { return m_screenPlayWidgetList; }

void ScreenPlay::createWallpaper(
        const int monitorIndex, QUrl absoluteStoragePath,
        const QString& previewImage, const float volume,
        const QString& fillMode, const QString& type)
{
    ProjectFile project{};
    if (!m_ilm->getProjectByAbsoluteStoragePath(&absoluteStoragePath, &project)) {
        return;
    }
    // Remove previous wallpaper, if any
    //this->removeAllWallpaper();
    this->removeWallpaperAt(0);
    m_settings->increaseActiveWallpaperCounter();

    m_screenPlayWallpaperList.emplace_back(
                RefSPWall::create(
                    std::vector<int>{monitorIndex}, absoluteStoragePath.toLocalFile(),
                    previewImage, volume, fillMode, type, this)
                );

    m_mlm->setWallpaperActiveMonitor(m_qGuiApplication->screens().at(monitorIndex),
                                     QString{absoluteStoragePath.toLocalFile() + "/" + previewImage});
}

void ScreenPlay::createWidget(QUrl absoluteStoragePath, const QString& previewImage)
{
    ProjectFile project{};
    if (!m_ilm->getProjectByAbsoluteStoragePath(&absoluteStoragePath, &project)) {
        return;
    }
    m_screenPlayWidgetList.emplace_back(
                RefSPWidget::create(
                    absoluteStoragePath.toLocalFile(), previewImage,
                    QString{absoluteStoragePath.toLocalFile() + "/" + project.m_file.toString()},
                    this)
                );
}

void ScreenPlay::removeAllWallpaper() noexcept
{
    if(m_sdkc && m_settings && !m_screenPlayWallpaperList.empty()){
        m_sdkc->closeAllWallpapers();
        m_settings->setActiveWallpaperCounter(0);
        m_screenPlayWallpaperList.clear();
        emit allWallpaperRemoved();
    }
    return;
}

void ScreenPlay::requestProjectSettingsListModelAt(const int index) const noexcept
{
    for (const RefSPWall& refSPWallpaper: m_screenPlayWallpaperList) {
        if (!refSPWallpaper.data()->screenNumber().empty() &&
                refSPWallpaper.data()->screenNumber()[0] == index) { // ??? only at index == 0
            emit projectSettingsListModelFound(
                        refSPWallpaper.data()->projectSettingsListModel().data(),
                        refSPWallpaper.data()->type());
            return;
        }
    }
    emit projectSettingsListModelNotFound();
}

void ScreenPlay::setWallpaperValue(const int at, const QString& key, const QString& value) noexcept
{
    Q_ASSERT(static_cast<std::size_t>(at) < m_screenPlayWallpaperList.size() && m_sdkc);
    for (const RefSPWall& refSPWallpaper: m_screenPlayWallpaperList) {
        if (!refSPWallpaper.data()->screenNumber().empty() && m_sdkc &&
                refSPWallpaper.data()->screenNumber()[0] == at) { // ??? only at index == 0
            m_sdkc->setWallpaperValue(refSPWallpaper.data()->appID(), key, value);
            return;
        }
    }
}

void ScreenPlay::setAllWallpaperValue(const QString& key, const QString& value) noexcept
{
    Q_ASSERT(m_sdkc);
    for (const RefSPWall& refSPWallpaper: m_screenPlayWallpaperList) {
        if(m_sdkc) m_sdkc->setWallpaperValue(refSPWallpaper.data()->appID(), key, value);
    }
}

void ScreenPlay::removeWallpaperAt(const int at)
{
    // Q_ASSERT(at < m_screenPlayWallpaperList.size() && m_sdkc);
    qDebug() << "No of walls in list: " << m_screenPlayWallpaperList.size();
    if(m_screenPlayWallpaperList.empty()) return; // done here;

    const auto toRemove = std::remove_if(
                m_screenPlayWallpaperList.begin(), m_screenPlayWallpaperList.end(),
                [&](const RefSPWall& refSPWallpaper) noexcept ->bool {
        const std::vector<int>& screenNumber = refSPWallpaper->screenNumber();
        qDebug() << "Screen No. :" << screenNumber.size();
        const bool isFound = !screenNumber.empty();// && screenNumber[0] == at;
        if(isFound) {
            // m_mlm
            m_sdkc->closeWallpapersAt(at); // for waht ???
            m_settings->decreaseActiveWallpaperCounter();
            qDebug() << "current wall count... " << m_settings->activeWallpaperCounter();
        }
        return isFound;
    });
    m_screenPlayWallpaperList.erase(toRemove, m_screenPlayWallpaperList.end());
    qDebug() << "After removing: No of walls in list: " << m_screenPlayWallpaperList.size();

}

std::vector<int> ScreenPlay::getMonitorByAppID(const QString& appID) const
{
    for (const RefSPWall& refSPWallpaper: m_screenPlayWallpaperList) {
        if (refSPWallpaper.data()->appID() == appID) {
            return refSPWallpaper.data()->screenNumber();
        }
    }
    return std::vector<int>{};
}

QString ScreenPlay::generateID() const
{
    const QString possibleCharacters{
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"};
    const int randomStringLength = 32;
    const auto radomGen = QRandomGenerator::system();

    QString randomString;
    for (int i = 0; i < randomStringLength; ++i) {
        const int index = radomGen->bounded(possibleCharacters.length());
        const QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}


ScreenPlayWallpaper::ScreenPlayWallpaper(const std::vector<int>& screenNumber, const QString& projectPath,
                                         const QString& previewImage, const float volume,
                                         const QString& fillMode, const QString& type, ScreenPlay *parent)
    : QObject{parent},
      m_screenNumber{std::move(screenNumber)},
      m_projectPath{projectPath},
      m_previewImage{previewImage},
      m_type{type},
      m_appID{ parent ? parent->generateID() : QString{} },
      m_process{nullptr},
      m_projectSettingsListModel{QSharedPointer<ProjectSettingsListModel>::create(projectPath + "/project.json")}
{
    QStringList proArgs;
    proArgs.append(QString::number(m_screenNumber.empty() ? 0 : m_screenNumber[0]));
    proArgs.append(m_projectPath);
    proArgs.append("appID=" + m_appID);
    proArgs.append(parent ? parent->settings()->decoder() : QString{});
    proArgs.append(QString::number(static_cast<double>(volume)));
    proArgs.append(fillMode);

    qDebug() << proArgs;

    // We do not want to parent the QProcess because the
    // Process manages its lifetime and destructing (animation) itself
    // via a disconnection from the ScreenPlay SDK
    QProcess* m_process = new QProcess();

    QObject::connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                     [=](int exitCode, QProcess::ExitStatus exitStatus)
                     {
                         if (exitCode != 0)
                             qDebug() << "WARNING EXIT CODE: " << exitCode;
                     });
    QObject::connect(m_process, &QProcess::errorOccurred, [=](QProcess::ProcessError error) {
                         qDebug() << "EX: " << error;
                     });

    m_process->setArguments(proArgs);
    m_process->setProgram(parent->settings()->screenPlayWindowPath().toString());
    m_process->start();
}

ScreenPlayWallpaper::~ScreenPlayWallpaper()
{
    if(m_process) delete m_process;
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

ScreenPlayWidget::ScreenPlayWidget(const QString& projectPath, const QString& previewImage,
                                   const QString& fullPath, ScreenPlay *parent)
    : m_projectPath{projectPath},
      m_previewImage{previewImage},
      m_fullPath{fullPath},
      m_appID{parent ? parent->generateID() : QString{}},
      m_position{0, 0},
      m_process{nullptr}
{
     QStringList proArgs;
     proArgs.append(m_projectPath);
     proArgs.append(m_appID);

     m_process = new QProcess(this); //PLS LESS BEHINDERT @Elias
     m_process->setArguments(proArgs);

     if (fullPath.endsWith(".exe")) {
        m_process->setProgram(fullPath);
     } else if (fullPath.endsWith(".qml")) {
        m_process->setProgram(parent->settings()->getScreenPlayWidgetPath().path());
     }

     qDebug() << m_process->program();

     QObject::connect(m_process, &QProcess::errorOccurred, this, [](QProcess::ProcessError error) {
     qDebug() << "error: " << error;
     });
     m_process->start();
}

ScreenPlayWidget::~ScreenPlayWidget()
{
    if(m_process) delete m_process;
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
