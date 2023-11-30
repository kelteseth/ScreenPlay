Create icons. Replace `\` with `^` on windows.

```
convert app.png -resize 16x16 ScreenPlay.iconset/icon_16x16.png \
&& convert app.png -resize 32x32 ScreenPlay.iconset/icon_16x16@2x.png \
&& convert app.png -resize 32x32 ScreenPlay.iconset/icon_32x32.png \
&& convert app.png -resize 64x64 ScreenPlay.iconset/icon_32x32@2x.png \
&& convert app.png -resize 128x128 ScreenPlay.iconset/icon_128x128.png \
&& convert app.png -resize 256x256 ScreenPlay.iconset/icon_128x128@2x.png \
&& convert app.png -resize 256x256 ScreenPlay.iconset/icon_256x256.png \
&& convert app.png -resize 512x512 ScreenPlay.iconset/icon_256x256@2x.png \
&& convert app.png -resize 512x512 ScreenPlay.iconset/icon_512x512.png \
&& convert app.png -resize 1024x1024 ScreenPlay.iconset/icon_512x512@2x.png

// Convert to .icns
iconutil -c icns ScreenPlay.iconset
```
Convert for Windows .ico
```
magick convert app.png \
\( -clone 0 -resize 16x16 \) \
\( -clone 0 -resize 32x32 \) \
\( -clone 0 -resize 48x48 \) \
\( -clone 0 -resize 64x64 \) \
\( -clone 0 -resize 128x128 \) \
\( -clone 0 -resize 256x256 \) \
-delete 0 -background transparent app.ico

```
Make the 1k logo smaller
```
convert app.png -resize 512x512 app_icon.png
```