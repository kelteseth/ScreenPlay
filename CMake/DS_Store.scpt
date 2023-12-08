on run argv
    -- Constants for window dimensions
    
    set BG_W to 780
    set BG_H to 390
    set TITLE_BAR_H to 22

    set diskImage to item 1 of argv

    tell application "Finder"
        tell disk diskImage
            -- Must call open and close to set window options later
            open
            -- Setup background and icon arrangement
            set current view of container window to icon view
            set theViewOptions to the icon view options of container window
            -- Note: CMake automatically renames the file to background.<ext>
            set background picture of theViewOptions to file ".background:background.png"
            set arrangement of theViewOptions to not arranged
            set icon size of theViewOptions to 72
            
            -- Attempt to hide sidebar and toolbar
            set toolbar visible of container window to false
            set statusbar visible of container window to false
            set sidebar width of container window to 0

            -- Get current position of the window
            set {currentX, currentY} to position of container window
            -- Set the new bounds of the window
            set bounds of container window to {currentX, currentY, currentX + BG_W, currentY + BG_H + TITLE_BAR_H}
            -- Set icon positions
            set position of item "ScreenPlay.app" to {137, 99}
            set position of item "Applications" to {287, 99}

            -- Update and apply changes
            update without registering applications
            delay 5
            close
        end tell
    end tell
end run
