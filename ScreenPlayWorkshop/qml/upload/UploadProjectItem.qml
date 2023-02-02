import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Controls.Material.impl
import ScreenPlayWorkshop
import WorkshopEnums

Page {
    id: root

    property string previewImagePath
    property real progress: 0.5
    property string name: "Headline"
    property var steamStatus

    width: 800
    height: 240
    anchors.centerIn: parent
    padding: 20
    onPreviewImagePathChanged: img.source = Qt.resolvedUrl("file:///" + previewImagePath)
    // Everyting that is not OK is a fail. See steam_qt_enums_generated.h
    onSteamStatusChanged: {
        let errorText;
        switch (steamStatus) {
        case SteamEnums.K_EResultNone:
            root.contentItem.state = "uploadComplete";
            return;
        case SteamEnums.K_EResultOK:
            root.contentItem.state = "uploadComplete";
            return;
        case SteamEnums.K_EResultFail:
            errorText = qsTr("Fail");
            break;
        case SteamEnums.K_EResultNoConnection:
            errorText = qsTr("No Connection");
            break;
        case SteamEnums.K_EResultInvalidPassword:
            errorText = qsTr("Invalid Password");
            break;
        case SteamEnums.K_EResultLoggedInElsewhere:
            errorText = qsTr("Logged In Elsewhere");
            break;
        case SteamEnums.K_EResultInvalidProtocolVer:
            errorText = qsTr("Invalid Protocol Version");
            break;
        case SteamEnums.K_EResultInvalidParam:
            errorText = qsTr("Invalid Param");
            break;
        case SteamEnums.K_EResultFileNotFound:
            errorText = qsTr("File Not Found");
            break;
        case SteamEnums.K_EResultBusy:
            errorText = qsTr("Busy");
            break;
        case SteamEnums.K_EResultInvalidState:
            errorText = qsTr("Invalid State");
            break;
        case SteamEnums.K_EResultInvalidName:
            errorText = qsTr("Invalid Name");
            break;
        case SteamEnums.K_EResultInvalidEmail:
            errorText = qsTr("Invalid Email");
            break;
        case SteamEnums.K_EResultDuplicateName:
            errorText = qsTr("Duplicate Name");
            break;
        case SteamEnums.K_EResultAccessDenied:
            errorText = qsTr("Access Denied");
            break;
        case SteamEnums.K_EResultTimeout:
            errorText = qsTr("Timeout");
            break;
        case SteamEnums.K_EResultBanned:
            errorText = qsTr("Banned");
            break;
        case SteamEnums.K_EResultAccountNotFound:
            errorText = qsTr("Account Not Found");
            break;
        case SteamEnums.K_EResultInvalidSteamID:
            errorText = qsTr("Invalid SteamID");
            break;
        case SteamEnums.K_EResultServiceUnavailable:
            errorText = qsTr("Service Unavailable");
            break;
        case SteamEnums.K_EResultNotLoggedOn:
            errorText = qsTr("Not Logged On");
            break;
        case SteamEnums.K_EResultPending:
            errorText = qsTr("Pending");
            break;
        case SteamEnums.K_EResultEncryptionFailure:
            errorText = qsTr("Encryption Failure");
            break;
        case SteamEnums.K_EResultInsufficientPrivilege:
            errorText = qsTr("Insufficient Privilege");
            break;
        case SteamEnums.K_EResultLimitExceeded:
            errorText = qsTr("Limit Exceeded");
            break;
        case SteamEnums.K_EResultRevoked:
            errorText = qsTr("Revoked");
            break;
        case SteamEnums.K_EResultExpired:
            errorText = qsTr("Expired");
            break;
        case SteamEnums.K_EResultAlreadyRedeemed:
            errorText = qsTr("Already Redeemed");
            break;
        case SteamEnums.K_EResultDuplicateRequest:
            errorText = qsTr("Duplicate Request");
            break;
        case SteamEnums.K_EResultAlreadyOwned:
            errorText = qsTr("Already Owned");
            break;
        case SteamEnums.K_EResultIPNotFound:
            errorText = qsTr("IP Not Found");
            break;
        case SteamEnums.K_EResultPersistFailed:
            errorText = qsTr("Persist Failed");
            break;
        case SteamEnums.K_EResultLockingFailed:
            errorText = qsTr("Locking Failed");
            break;
        case SteamEnums.K_EResultLogonSessionReplaced:
            errorText = qsTr("Logon Session Replaced");
            break;
        case SteamEnums.K_EResultConnectFailed:
            errorText = qsTr("Connect Failed");
            break;
        case SteamEnums.K_EResultHandshakeFailed:
            errorText = qsTr("Handshake Failed");
            break;
        case SteamEnums.K_EResultIOFailure:
            errorText = qsTr("IO Failure");
            break;
        case SteamEnums.K_EResultRemoteDisconnect:
            errorText = qsTr("Remote Disconnect");
            break;
        case SteamEnums.K_EResultShoppingCartNotFound:
            errorText = qsTr("Shopping Cart Not Found");
            break;
        case SteamEnums.K_EResultBlocked:
            errorText = qsTr("Blocked");
            break;
        case SteamEnums.K_EResultIgnored:
            errorText = qsTr("Ignored");
            break;
        case SteamEnums.K_EResultNoMatch:
            errorText = qsTr("No Match");
            break;
        case SteamEnums.K_EResultAccountDisabled:
            errorText = qsTr("Account Disabled");
            break;
        case SteamEnums.K_EResultServiceReadOnly:
            errorText = qsTr("Service ReadOnly");
            break;
        case SteamEnums.K_EResultAccountNotFeatured:
            errorText = qsTr("Account Not Featured");
            break;
        case SteamEnums.K_EResultAdministratorOK:
            errorText = qsTr("Administrator OK");
            break;
        case SteamEnums.K_EResultContentVersion:
            errorText = qsTr("Content Version");
            break;
        case SteamEnums.K_EResultTryAnotherCM:
            errorText = qsTr("Try Another CM");
            break;
        case SteamEnums.K_EResultPasswordRequiredToKickSession:
            errorText = qsTr("Password Required To Kick Session");
            break;
        case SteamEnums.K_EResultAlreadyLoggedInElsewhere:
            errorText = qsTr("Already Logged In Elsewhere");
            break;
        case SteamEnums.K_EResultSuspended:
            errorText = qsTr("Suspended");
            break;
        case SteamEnums.K_EResultCancelled:
            errorText = qsTr("Cancelled");
            break;
        case SteamEnums.K_EResultDataCorruption:
            errorText = qsTr("Data Corruption");
            break;
        case SteamEnums.K_EResultDiskFull:
            errorText = qsTr("Disk Full");
            break;
        case SteamEnums.K_EResultRemoteCallFailed:
            errorText = qsTr("Remote Call Failed");
            break;
        case SteamEnums.K_EResultPasswordUnset:
            errorText = qsTr("Password Unset");
            break;
        case SteamEnums.K_EResultExternalAccountUnlinked:
            errorText = qsTr("External Account Unlinked");
            break;
        case SteamEnums.K_EResultPSNTicketInvalid:
            errorText = qsTr("PSN Ticket Invalid");
            break;
        case SteamEnums.K_EResultExternalAccountAlreadyLinked:
            errorText = qsTr("External Account Already Linked");
            break;
        case SteamEnums.K_EResultRemoteFileConflict:
            errorText = qsTr("Remote File Conflict");
            break;
        case SteamEnums.K_EResultIllegalPassword:
            errorText = qsTr("Illegal Password");
            break;
        case SteamEnums.K_EResultSameAsPreviousValue:
            errorText = qsTr("Same As Previous Value");
            break;
        case SteamEnums.K_EResultAccountLogonDenied:
            errorText = qsTr("Account Logon Denied");
            break;
        case SteamEnums.K_EResultCannotUseOldPassword:
            errorText = qsTr("Cannot Use Old Password");
            break;
        case SteamEnums.K_EResultInvalidLoginAuthCode:
            errorText = qsTr("Invalid Login AuthCode");
            break;
        case SteamEnums.K_EResultAccountLogonDeniedNoMail:
            errorText = qsTr("Account Logon Denied No Mail");
            break;
        case SteamEnums.K_EResultHardwareNotCapableOfIPT:
            errorText = qsTr("Hardware Not Capable Of IPT");
            break;
        case SteamEnums.K_EResultIPTInitError:
            errorText = qsTr("IPT Init Error");
            break;
        case SteamEnums.K_EResultParentalControlRestricted:
            errorText = qsTr("Parental Control Restricted");
            break;
        case SteamEnums.K_EResultFacebookQueryError:
            errorText = qsTr("Facebook Query Error");
            break;
        case SteamEnums.K_EResultExpiredLoginAuthCode:
            errorText = qsTr("Expired Login Auth Code");
            break;
        case SteamEnums.K_EResultIPLoginRestrictionFailed:
            errorText = qsTr("IP Login Restriction Failed");
            break;
        case SteamEnums.K_EResultAccountLockedDown:
            errorText = qsTr("Account Locked Down");
            break;
        case SteamEnums.K_EResultAccountLogonDeniedVerifiedEmailRequired:
            errorText = qsTr("Account Logon Denied Verified Email Required");
            break;
        case SteamEnums.K_EResultNoMatchingURL:
            errorText = qsTr("No MatchingURL");
            break;
        case SteamEnums.K_EResultBadResponse:
            errorText = qsTr("Bad Response");
            break;
        case SteamEnums.K_EResultRequirePasswordReEntry:
            errorText = qsTr("Require Password ReEntry");
            break;
        case SteamEnums.K_EResultValueOutOfRange:
            errorText = qsTr("Value Out Of Range");
            break;
        case SteamEnums.K_EResultUnexpectedError:
            errorText = qsTr("Unexpecte Error");
            break;
        case SteamEnums.K_EResultDisabled:
            errorText = qsTr("Disabled");
            break;
        case SteamEnums.K_EResultInvalidCEGSubmission:
            errorText = qsTr("Invalid CEG Submission");
            break;
        case SteamEnums.K_EResultRestrictedDevice:
            errorText = qsTr("Restricted Device");
            break;
        case SteamEnums.K_EResultRegionLocked:
            errorText = qsTr("Region Locked");
            break;
        case SteamEnums.K_EResultRateLimitExceeded:
            errorText = qsTr("Rate Limit Exceeded");
            break;
        case SteamEnums.K_EResultAccountLoginDeniedNeedTwoFactor:
            errorText = qsTr("Account Login Denied Need Two Factor");
            break;
        case SteamEnums.K_EResultItemDeleted:
            errorText = qsTr("Item Deleted");
            break;
        case SteamEnums.K_EResultAccountLoginDeniedThrottle:
            errorText = qsTr("Account Login Denied Throttle");
            break;
        case SteamEnums.K_EResultTwoFactorCodeMismatch:
            errorText = qsTr("Two Factor Code Mismatch");
            break;
        case SteamEnums.K_EResultTwoFactorActivationCodeMismatch:
            errorText = qsTr("Two Factor Activation Code Mismatch");
            break;
        case SteamEnums.K_EResultAccountAssociatedToMultiplePartners:
            errorText = qsTr("Account Associated To Multiple Partners");
            break;
        case SteamEnums.K_EResultNotModified:
            errorText = qsTr("Not Modified");
            break;
        case SteamEnums.K_EResultNoMobileDevice:
            errorText = qsTr("No Mobile Device");
            break;
        case SteamEnums.K_EResultTimeNotSynced:
            errorText = qsTr("Time Not Synced");
            break;
        case SteamEnums.K_EResultSmsCodeFailed:
            errorText = qsTr("Sms Code Failed");
            break;
        case SteamEnums.K_EResultAccountLimitExceeded:
            errorText = qsTr("Account Limit Exceeded");
            break;
        case SteamEnums.K_EResultAccountActivityLimitExceeded:
            errorText = qsTr("Account Activity Limit Exceeded");
            break;
        case SteamEnums.K_EResultPhoneActivityLimitExceeded:
            errorText = qsTr("Phone Activity Limit Exceeded");
            break;
        case SteamEnums.K_EResultRefundToWallet:
            errorText = qsTr("Refund To Wallet");
            break;
        case SteamEnums.K_EResultEmailSendFailure:
            errorText = qsTr("Email Send Failure");
            break;
        case SteamEnums.K_EResultNotSettled:
            errorText = qsTr("Not Settled");
            break;
        case SteamEnums.K_EResultNeedCaptcha:
            errorText = qsTr("Need Captcha");
            break;
        case SteamEnums.K_EResultGSLTDenied:
            errorText = qsTr("GSLT Denied");
            break;
        case SteamEnums.K_EResultGSOwnerDenied:
            errorText = qsTr("GS Owner Denied");
            break;
        case SteamEnums.K_EResultInvalidItemType:
            errorText = qsTr("Invalid Item Type");
            break;
        case SteamEnums.K_EResultIPBanned:
            errorText = qsTr("IP Banned");
            break;
        case SteamEnums.K_EResultGSLTExpired:
            errorText = qsTr("GSLT Expired");
            break;
        case SteamEnums.K_EResultInsufficientFunds:
            errorText = qsTr("Insufficient Funds");
            break;
        case SteamEnums.K_EResultTooManyPending:
            errorText = qsTr("Too Many Pending");
            break;
        case SteamEnums.K_EResultNoSiteLicensesFound:
            errorText = qsTr("No Site Licenses Found");
            break;
        case SteamEnums.K_EResultWGNetworkSendExceeded:
            errorText = qsTr("WG Network Send Exceeded");
            break;
        case SteamEnums.K_EResultAccountNotFriends:
            errorText = qsTr("Account Not Friends");
            break;
        case SteamEnums.K_EResultLimitedUserAccount:
            errorText = qsTr("Limited User Account");
            break;
        case SteamEnums.K_EResultCantRemoveItem:
            errorText = qsTr("Cant Remove Item");
            break;
        case SteamEnums.K_EResultAccountDeleted:
            errorText = qsTr("Account Deleted");
            break;
        case SteamEnums.K_EResultExistingUserCancelledLicense:
            errorText = qsTr("Existing User Cancelled License");
            break;
        case SteamEnums.K_EResultCommunityCooldown:
            errorText = qsTr("Community Cooldown");
            break;
        }
        root.contentItem.txtStatus.statusText = errorText;
        root.contentItem.state = "error";
    }

    background: Rectangle {
        radius: 4
        anchors.fill: parent
        layer.enabled: true
        color: Material.theme === Material.Light ? "white" : Material.background

        layer.effect: ElevationEffect {
            elevation: 6
        }
    }

    contentItem: Item {
        anchors.fill: parent
        state: "base"

        Image {
            id: img

            width: 300

            anchors {
                top: parent.top
                left: parent.left
                bottom: parent.bottom
            }

            LinearGradient {
                id: gradient

                height: parent.height
                cached: true
                opacity: 0
                anchors.fill: parent
                start: Qt.point(0, height)
                end: Qt.point(0, 0)

                gradient: Gradient {
                    GradientStop {
                        id: gradientStop0

                        position: 0
                        color: "#DD000000"
                    }

                    GradientStop {
                        id: gradientStop1

                        position: 1
                        color: "#00000000"
                    }
                }
            }
        }

        ColumnLayout {
            spacing: 10

            anchors {
                top: parent.top
                right: parent.right
                left: img.right
                margins: 20
            }

            Text {
                id: name

                text: root.name
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                color: Material.primaryTextColor
                font.pointSize: 18
                Layout.preferredHeight: 30
                Layout.fillWidth: true
            }

            Text {
                id: txtStatus

                property string statusText: "Loading..."

                text: qsTr("Status:") + " " + statusText
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                color: Material.primaryTextColor
                font.pointSize: 14
                Layout.preferredHeight: 30
            }

            Item {
                Layout.preferredHeight: 60
                Layout.fillWidth: true
            }

            ColumnLayout {
                spacing: 10
                Layout.fillWidth: true

                Text {
                    text: qsTr("Upload Progress: ") + " " + Math.ceil(root.progress) + "%"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: Material.primaryTextColor
                    font.pointSize: 14
                    height: 50
                }

                ProgressBar {
                    id: progressBar

                    Layout.fillWidth: true
                    value: root.progress
                    to: 100
                }
            }
        }

        states: [
            State {
                name: "uploading"

                PropertyChanges {
                }
            },
            State {
                name: "uploadComplete"

                PropertyChanges {
                    target: gradient
                    opacity: 0.7
                }

                PropertyChanges {
                    target: gradient
                    opacity: 0.7
                }

                PropertyChanges {
                    target: gradientStop0
                    color: Material.color(Material.Lime)
                }

                PropertyChanges {
                    target: gradientStop1
                    color: Material.color(Material.LightGreen)
                }
            },
            State {
                name: "error"

                PropertyChanges {
                    target: gradient
                    opacity: 0.7
                }

                PropertyChanges {
                    target: gradientStop0
                    color: Material.color(Material.Red)
                }

                PropertyChanges {
                    target: gradientStop1
                    color: Material.color(Material.DeepOrange)
                }
            }
        ]
        transitions: [
            Transition {
                from: "base"
                to: "uploading"

                PropertyAnimation {
                    targets: [gradient, gradientStop0, gradientStop1]
                    duration: 500
                }
            }
        ]
    }
}
