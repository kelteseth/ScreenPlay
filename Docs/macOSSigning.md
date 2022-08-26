# ScreenPlay macOS signing full guide

## Create a developer account and certs
1. Pay the 99$ Apple tax
2. Create a app password for distribution outside of the app store
- Go to https://appleid.apple.com/account/manage
- Call it ScreenPlay and save the password!

## Add a new device
Based on: https://developer.apple.com/forums/thread/699268

1. You run Keychain Access and choose Certificate Assistant > Request a Certificate from a Certificate Authority.
1. You run through the workflow as described in Developer [Account Help > Create certificates > Create a certificate signing](https://help.apple.com/developer-account/#/devbfa00fef7) request. This does two things:
    - It generates a public / private key pair in your keychain. To see these, run Keychain Access and select “login” on the left and Keys at the top. Look for keys whose names match the Common Name you entered in step 2.
    - It prompts you to save a .certSigningRequest file (CSR). This contains a copy of the public key.
1. You upload the CSR file to the [developer web site](https://developer.apple.com/account/resources/certificates/list). Select `Developer ID Application` and upload your new `CertificateSigningRequest.certSigningRequest`.
1. The developer web site issues you a certificate. In human terms this certificate says “Apple certifies that the subject of this certificate holds the private key that matches the public key embedded in this certificate.”

> Note The developer web site sets the subject information in the certificate based on your developer account. It ignores the subject information in the CSR. So, you can enter any information you want in step 2. This is a good way to distinguish between different keys in your keychain. For example, you might set the Common Name field in step 2 to include a unique identifier that allows you to easily identify the public / private key pair generated in step 3.

5. Download the certificate and add it to your keychain.

## Testing 
There should be at least one valid identity:
```
security find-identity -p codesigning -v
```

should print:

```
1) xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx "Developer ID Application: Elias Steurer (V887LHYKRH)"
     1 valid identities found
```

## Sign the app locally with codesign
We need to sign every single file in the .app file. For this we use the name from the installed cert. This can be copied from the `Keychain Access.app`.

```
codesign --deep -f -s "Developer ID Application: Elias Steurer (V887LHYKRH)" --options "runtime" "ScreenPlay.app/"
```

## Add App Store Connect API private key:
- [Create Private Key](https://appstoreconnect.apple.com/access/users)
- Save private key as a file, KEY ID, Issuer ID. We need them next:

```
xcrun notarytool  store-credentials
```

1. Profile name: 
    - Profile name: tachiom
2. Path to App Store Connect API private key:
    - `/Users/eliassteurer/Documents/AuthKey_xxxxxxx.p8`
3. App Store Connect API Key ID:
    - KEY ID at: https://appstoreconnect.apple.com/access/api
4. App Store Connect API Issuer ID:
    - USER ID at: https://appstoreconnect.apple.com/access/api


## Get an App-Specific Password
https://stackoverflow.com/questions/56890749/macos-notarize-in-script
```
security add-generic-password -a "kelteseth@gmail.com" -w "xxxx-xxx-xxx-xxx" -s "Developer ID Application: Elias Steurer (V887LHYKRH)"
```

## Upload to apple for notization
We use [xcnotary](https://github.com/akeru-inc/xcnotary) tools for fast automatic upload. Install it via brew:

`brew install akeru-inc/tap/xcnotary`

Then run it with the 
- `*.app` name
- `-d` the developer account email and 
- `-k` command is here the keychain name that contains your password from the app password step above!

`xcnotary notarize ScreenPlay.app -d yourDeveloperAccountEmail@example.com -k ScreenPlay`

