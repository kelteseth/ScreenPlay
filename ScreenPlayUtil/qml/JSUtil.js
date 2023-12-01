function isWallpaper(type) {
    return type === ContentTypes.InstalledType.VideoWallpaper
            || type === ContentTypes.InstalledType.HTMLWallpaper
            || type === ContentTypes.InstalledType.QMLWallpaper
            || type === ContentTypes.InstalledType.GifWallpaper
            || type === ContentTypes.InstalledType.WebsiteWallpaper
            || type === ContentTypes.InstalledType.GodotWallpaper
}

function isWidget(type) {
    return type === ContentTypes.InstalledType.HTMLWidget || type === ContentTypes.InstalledType.QMLWidget
}

function isScene(type) {
    return type === ContentTypes.InstalledType.HTMLWallpaper
            || type === ContentTypes.InstalledType.QMLWallpaper
            || type === ContentTypes.InstalledType.WebsiteWallpaper
            || type === ContentTypes.InstalledType.GodotWallpaper
}

function isVideo(type) {
    return type === ContentTypes.InstalledType.VideoWallpaper
            || type === ContentTypes.InstalledType.GifWallpaper
}

