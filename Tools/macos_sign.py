#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
from build import BuildConfig
from util import  run
from sys import stdout
import time

stdout.reconfigure(encoding='utf-8')

def sign(build_config: BuildConfig):
    print("Run codedesign")
    #run("codesign -f -s 'Developer ID Application: Elias Steurer (V887LHYKRH)' --verbose --force  --timestamp --options 'runtime' -f --entitlements '../../ScreenPlay/entitlements.plist' 'ScreenPlay.app/' ", 
    #    cwd=build_config.bin_dir)
    # Do not use --deep https://developer.apple.com/forums/thread/129980
    # base_sign_command = "codesign  -s \"Developer ID Application: Elias Steurer (V887LHYKRH)\" --verbose --force --timestamp --options \"runtime\" \"ScreenPlay.app/Contents/MacOS/{app}\""
    # run(base_sign_command.format(app="ffmpeg"),  cwd=build_config.bin_dir)
    # run(base_sign_command.format(app="ffprobe"), cwd=build_config.bin_dir)
    run("codesign --deep -s \"Developer ID Application: Elias Steurer (V887LHYKRH)\" --verbose --force --timestamp --options \"runtime\" --entitlements \"../../ScreenPlay/entitlements.plist\"  \"ScreenPlay.app/\"", 
        cwd=build_config.bin_dir)

    print("Run codedesign verify")
    run("codesign --verify --verbose=4  'ScreenPlay.app/'", 
        cwd=build_config.bin_dir)

    # Note the profile is the one name of the first step of (App Store Connect API) in the macOSSigning.md
    # xcrun notarytool submit "ScreenPlay.app.zip" --keychain-profile "ScreenPlay" --wait
    # xcrun stapler staple "ScreenPlay.app"
    print("Packing .apps for upload")
    run("ditto -c -k --keepParent 'ScreenPlay.app' 'ScreenPlay.app.zip'", cwd=build_config.bin_dir)

    # run this  if you get an error:
    # `xcrun notarytool log --apple-id "xxxxx@xxxx.com"  --password "xxxx-xxxx-xxxx-xxxx" --team-id "xxxxxxxxxxx"  <ID>`
    # Processing complete
    # id: xxxxxx-xxxxxx-xxxx-xxxxx-xxxxx
    # status: Invalid
    print("Run xcnotary submit")
    run("xcrun notarytool submit --keychain-profile 'ScreenPlay' ScreenPlay.app.zip  --wait", cwd=build_config.bin_dir)

    print("Run stapler staple")
    run("xcrun stapler staple ScreenPlay.app", cwd=build_config.bin_dir)
    print("Run spctl assess")
    run("spctl --assess --verbose  'ScreenPlay.app/'", cwd=build_config.bin_dir)

    print("Remove *.app.zip files.")
    run("rm ScreenPlay.app.zip",  cwd=build_config.bin_dir)
    

    # We also need to sign the installer in osx:
    if build_config.create_installer == "ON":
        run("codesign --deep -f -s \"Developer ID Application: Elias Steurer (V887LHYKRH)\" --timestamp --options \"runtime\" -f --deep \"ScreenPlay-Installer.dmg/ScreenPlay-Installer.app/Contents/MacOS/ScreenPlay-Installer\"", cwd=build_config.build_folder)
        run("codesign --verify --verbose=4  \"ScreenPlay-Installer.dmg/ScreenPlay-Installer.app/Contents/MacOS/ScreenPlay-Installer\"",
            cwd=build_config.build_folder)
        run("xcnotary notarize ScreenPlay-Installer.dmg/ScreenPlay-Installer.app -d kelteseth@gmail.com -k ScreenPlay",
            cwd=build_config.build_folder)
        run("spctl --assess --verbose  \"ScreenPlay-Installer.dmg/ScreenPlay-Installer.app/\"",
            cwd=build_config.build_folder)

        run("codesign --deep -f -s \"Developer ID Application: Elias Steurer (V887LHYKRH)\" --timestamp --options \"runtime\" -f --deep \"ScreenPlay-Installer.dmg/\"", cwd=build_config.build_folder)
        run("codesign --verify --verbose=4  \"ScreenPlay-Installer.dmg/\"",
            cwd=build_config.build_folder)
        run("xcnotary notarize ScreenPlay-Installer.dmg -d kelteseth@gmail.com -k ScreenPlay",
            cwd=build_config.build_folder)
        run("spctl --assess --verbose  \"ScreenPlay-Installer.dmg/\"",
            cwd=build_config.build_folder)

