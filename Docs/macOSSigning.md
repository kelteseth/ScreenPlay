# ScreenPlay macOS signing full guide

## Create a developer account and certs
1. Pay the 99$ Apple tax
2. Create a app password for distribution outside of the app store
- Go to https://appleid.apple.com/account/manage
- Call it ScreenPlay and save the password!
3. Next we need a developer certificate:
- Go to https://developer.apple.com/account/resources/certificates/list
- Click the blue plus sign
- Select `Developer ID Application`
    - _This certificate is used to code sign your app for distribution outside of the Mac App Store._

## Sign the app locally with codesign
We need to sign every single file in the .app file. For this we use the name from the installed cert. This can be copied from the `Keychain Access.app`.

`codesign --deep -f -s "Developer ID Application: Elias Steurer (AAABCXYZAA)" --options "runtime" "ScreenPlay.app/"`

## Upload to apple for notization
We use [xcnotary](https://github.com/akeru-inc/xcnotary) tools for fast automatic upload. Install it via brew:

`brew install akeru-inc/tap/xcnotary`

Then run it with the 
- `*.app` name
- `-d` the developer account email and 
- `-k` command is here the keychain name that contains your password from the app password step above!

`xcnotary notarize ScreenPlay.app -d yourDeveloperAccountEmail@example.com -k ScreenPlay`
