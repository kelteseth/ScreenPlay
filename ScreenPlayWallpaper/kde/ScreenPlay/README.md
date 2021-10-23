# ScreenPlay Wallpaper for KDE Plasma Desktop

One has to install it via the command below. Simply putting it into:
* ~/.local/share/plasma/wallpapers/
Will not work because KDE uses the kpluginindex.json (that is actually a bz2 file. Do not ask why...) to load all available wallpaper. 

#### Installation
```
sudo apt install qml-module-qt-websockets qtwebengine5-*

plasmapkg2 --install ScreenPlay
```

#### Application structure
Because Wallpaper and Widgets are already a different application we can extend the logic for KDE. For this we create a local websocket instance to communicate with our main ScreenPlay app.

### Development
1. Make changes
1. `plasmapkg2 --upgrade ScreenPlay`
1. Open Desktop Settings
    - Select Wallpaper type Image
1. Close Desktop Settings Window
1. Open Desktop Settings
    - Select Wallpaper type ScreenPlay

```
plasmapkg2 --upgrade ScreenPlay ; kquitapp5 plasmashell;  kstart5 plasmashell
```