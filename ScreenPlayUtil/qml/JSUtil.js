function isWallpaper(type) {
    return type === InstalledType.VideoWallpaper
            || type === InstalledType.HTMLWallpaper
            || type === InstalledType.QMLWallpaper
            || type === InstalledType.GifWallpaper
            || type === InstalledType.WebsiteWallpaper
            || type === InstalledType.GodotWallpaper
}

function isWidget(type) {
    return type === InstalledType.HTMLWidget || type === InstalledType.QMLWidget
}

function isScene(type) {
    return type === InstalledType.HTMLWallpaper
            || type === InstalledType.QMLWallpaper
            || type === InstalledType.WebsiteWallpaper
            || type === InstalledType.GodotWallpaper
}

function isVideo(type) {
    return type === InstalledType.VideoWallpaper
            || type === InstalledType.GifWallpaper
}

