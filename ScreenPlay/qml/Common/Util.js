
function isWallpaper(type) {
    return type === InstalledType.VideoWallpaper
            || type === InstalledType.HTMLWallpaper
            || type === InstalledType.QMLWallpaper
            || type === InstalledType.GifWallpaper
            || type === InstalledType.WebsiteWallpaper
            || type === InstalledType.GodotWallpaper
}

function isWidget(type) {
    return type === InstalledType.HTMLWidget
            || type === InstalledType.QMLWidget
}
