// GENERATED FILE DO NOT EDIT
// TO REGENERATE run ScreenPlayInternal\SteamSDK
// > python.exe updateEnums.py

#pragma once
#include <QObject>
#include <QQmlEngine>

namespace ScreenPlay {

class Steam : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")

public:
    explicit Steam();

    enum class ESteamIPType
{	
	K_ESteamIPTypeIPv4 = 0,
	K_ESteamIPTypeIPv6 = 1,
 };
Q_ENUM(ESteamIPType)

enum class EUniverse
{	
	K_EUniverseInvalid = 0,
	K_EUniversePublic = 1,
	K_EUniverseBeta = 2,
	K_EUniverseInternal = 3,
	K_EUniverseDev = 4,
	K_EUniverseMax = 5,
 };
Q_ENUM(EUniverse)

enum class EResult
{	
	K_EResultNone = 0,
	K_EResultOK = 1,
	K_EResultFail = 2,
	K_EResultNoConnection = 3,
	K_EResultInvalidPassword = 5,
	K_EResultLoggedInElsewhere = 6,
	K_EResultInvalidProtocolVer = 7,
	K_EResultInvalidParam = 8,
	K_EResultFileNotFound = 9,
	K_EResultBusy = 10,
	K_EResultInvalidState = 11,
	K_EResultInvalidName = 12,
	K_EResultInvalidEmail = 13,
	K_EResultDuplicateName = 14,
	K_EResultAccessDenied = 15,
	K_EResultTimeout = 16,
	K_EResultBanned = 17,
	K_EResultAccountNotFound = 18,
	K_EResultInvalidSteamID = 19,
	K_EResultServiceUnavailable = 20,
	K_EResultNotLoggedOn = 21,
	K_EResultPending = 22,
	K_EResultEncryptionFailure = 23,
	K_EResultInsufficientPrivilege = 24,
	K_EResultLimitExceeded = 25,
	K_EResultRevoked = 26,
	K_EResultExpired = 27,
	K_EResultAlreadyRedeemed = 28,
	K_EResultDuplicateRequest = 29,
	K_EResultAlreadyOwned = 30,
	K_EResultIPNotFound = 31,
	K_EResultPersistFailed = 32,
	K_EResultLockingFailed = 33,
	K_EResultLogonSessionReplaced = 34,
	K_EResultConnectFailed = 35,
	K_EResultHandshakeFailed = 36,
	K_EResultIOFailure = 37,
	K_EResultRemoteDisconnect = 38,
	K_EResultShoppingCartNotFound = 39,
	K_EResultBlocked = 40,
	K_EResultIgnored = 41,
	K_EResultNoMatch = 42,
	K_EResultAccountDisabled = 43,
	K_EResultServiceReadOnly = 44,
	K_EResultAccountNotFeatured = 45,
	K_EResultAdministratorOK = 46,
	K_EResultContentVersion = 47,
	K_EResultTryAnotherCM = 48,
	K_EResultPasswordRequiredToKickSession = 49,
	K_EResultAlreadyLoggedInElsewhere = 50,
	K_EResultSuspended = 51,
	K_EResultCancelled = 52,
	K_EResultDataCorruption = 53,
	K_EResultDiskFull = 54,
	K_EResultRemoteCallFailed = 55,
	K_EResultPasswordUnset = 56,
	K_EResultExternalAccountUnlinked = 57,
	K_EResultPSNTicketInvalid = 58,
	K_EResultExternalAccountAlreadyLinked = 59,
	K_EResultRemoteFileConflict = 60,
	K_EResultIllegalPassword = 61,
	K_EResultSameAsPreviousValue = 62,
	K_EResultAccountLogonDenied = 63,
	K_EResultCannotUseOldPassword = 64,
	K_EResultInvalidLoginAuthCode = 65,
	K_EResultAccountLogonDeniedNoMail = 66,
	K_EResultHardwareNotCapableOfIPT = 67,
	K_EResultIPTInitError = 68,
	K_EResultParentalControlRestricted = 69,
	K_EResultFacebookQueryError = 70,
	K_EResultExpiredLoginAuthCode = 71,
	K_EResultIPLoginRestrictionFailed = 72,
	K_EResultAccountLockedDown = 73,
	K_EResultAccountLogonDeniedVerifiedEmailRequired = 74,
	K_EResultNoMatchingURL = 75,
	K_EResultBadResponse = 76,
	K_EResultRequirePasswordReEntry = 77,
	K_EResultValueOutOfRange = 78,
	K_EResultUnexpectedError = 79,
	K_EResultDisabled = 80,
	K_EResultInvalidCEGSubmission = 81,
	K_EResultRestrictedDevice = 82,
	K_EResultRegionLocked = 83,
	K_EResultRateLimitExceeded = 84,
	K_EResultAccountLoginDeniedNeedTwoFactor = 85,
	K_EResultItemDeleted = 86,
	K_EResultAccountLoginDeniedThrottle = 87,
	K_EResultTwoFactorCodeMismatch = 88,
	K_EResultTwoFactorActivationCodeMismatch = 89,
	K_EResultAccountAssociatedToMultiplePartners = 90,
	K_EResultNotModified = 91,
	K_EResultNoMobileDevice = 92,
	K_EResultTimeNotSynced = 93,
	K_EResultSmsCodeFailed = 94,
	K_EResultAccountLimitExceeded = 95,
	K_EResultAccountActivityLimitExceeded = 96,
	K_EResultPhoneActivityLimitExceeded = 97,
	K_EResultRefundToWallet = 98,
	K_EResultEmailSendFailure = 99,
	K_EResultNotSettled = 100,
	K_EResultNeedCaptcha = 101,
	K_EResultGSLTDenied = 102,
	K_EResultGSOwnerDenied = 103,
	K_EResultInvalidItemType = 104,
	K_EResultIPBanned = 105,
	K_EResultGSLTExpired = 106,
	K_EResultInsufficientFunds = 107,
	K_EResultTooManyPending = 108,
	K_EResultNoSiteLicensesFound = 109,
	K_EResultWGNetworkSendExceeded = 110,
	K_EResultAccountNotFriends = 111,
	K_EResultLimitedUserAccount = 112,
	K_EResultCantRemoveItem = 113,
	K_EResultAccountDeleted = 114,
	K_EResultExistingUserCancelledLicense = 115,
	K_EResultCommunityCooldown = 116,
	K_EResultNoLauncherSpecified = 117,
	K_EResultMustAgreeToSSA = 118,
	K_EResultLauncherMigrated = 119,
	K_EResultSteamRealmMismatch = 120,
	K_EResultInvalidSignature = 121,
	K_EResultParseFailure = 122,
	K_EResultNoVerifiedPhone = 123,
	K_EResultInsufficientBattery = 124,
	K_EResultChargerRequired = 125,
	K_EResultCachedCredentialInvalid = 126,
	K_EResultPhoneNumberIsVOIP = 127,
 };
Q_ENUM(EResult)

enum class EVoiceResult
{	
	K_EVoiceResultOK = 0,
	K_EVoiceResultNotInitialized = 1,
	K_EVoiceResultNotRecording = 2,
	K_EVoiceResultNoData = 3,
	K_EVoiceResultBufferTooSmall = 4,
	K_EVoiceResultDataCorrupted = 5,
	K_EVoiceResultRestricted = 6,
	K_EVoiceResultUnsupportedCodec = 7,
	K_EVoiceResultReceiverOutOfDate = 8,
	K_EVoiceResultReceiverDidNotAnswer = 9,
 };
Q_ENUM(EVoiceResult)

enum class EDenyReason
{	
	K_EDenyInvalid = 0,
	K_EDenyInvalidVersion = 1,
	K_EDenyGeneric = 2,
	K_EDenyNotLoggedOn = 3,
	K_EDenyNoLicense = 4,
	K_EDenyCheater = 5,
	K_EDenyLoggedInElseWhere = 6,
	K_EDenyUnknownText = 7,
	K_EDenyIncompatibleAnticheat = 8,
	K_EDenyMemoryCorruption = 9,
	K_EDenyIncompatibleSoftware = 10,
	K_EDenySteamConnectionLost = 11,
	K_EDenySteamConnectionError = 12,
	K_EDenySteamResponseTimedOut = 13,
	K_EDenySteamValidationStalled = 14,
	K_EDenySteamOwnerLeftGuestUser = 15,
 };
Q_ENUM(EDenyReason)

enum class EBeginAuthSessionResult
{	
	K_EBeginAuthSessionResultOK = 0,
	K_EBeginAuthSessionResultInvalidTicket = 1,
	K_EBeginAuthSessionResultDuplicateRequest = 2,
	K_EBeginAuthSessionResultInvalidVersion = 3,
	K_EBeginAuthSessionResultGameMismatch = 4,
	K_EBeginAuthSessionResultExpiredTicket = 5,
 };
Q_ENUM(EBeginAuthSessionResult)

enum class EAuthSessionResponse
{	
	K_EAuthSessionResponseOK = 0,
	K_EAuthSessionResponseUserNotConnectedToSteam = 1,
	K_EAuthSessionResponseNoLicenseOrExpired = 2,
	K_EAuthSessionResponseVACBanned = 3,
	K_EAuthSessionResponseLoggedInElseWhere = 4,
	K_EAuthSessionResponseVACCheckTimedOut = 5,
	K_EAuthSessionResponseAuthTicketCanceled = 6,
	K_EAuthSessionResponseAuthTicketInvalidAlreadyUsed = 7,
	K_EAuthSessionResponseAuthTicketInvalid = 8,
	K_EAuthSessionResponsePublisherIssuedBan = 9,
	K_EAuthSessionResponseAuthTicketNetworkIdentityFailure = 10,
 };
Q_ENUM(EAuthSessionResponse)

enum class EUserHasLicenseForAppResult
{	
	K_EUserHasLicenseResultHasLicense = 0,
	K_EUserHasLicenseResultDoesNotHaveLicense = 1,
	K_EUserHasLicenseResultNoAuth = 2,
 };
Q_ENUM(EUserHasLicenseForAppResult)

enum class EAccountType
{	
	K_EAccountTypeInvalid = 0,
	K_EAccountTypeIndividual = 1,
	K_EAccountTypeMultiseat = 2,
	K_EAccountTypeGameServer = 3,
	K_EAccountTypeAnonGameServer = 4,
	K_EAccountTypePending = 5,
	K_EAccountTypeContentServer = 6,
	K_EAccountTypeClan = 7,
	K_EAccountTypeChat = 8,
	K_EAccountTypeConsoleUser = 9,
	K_EAccountTypeAnonUser = 10,
	K_EAccountTypeMax = 11,
 };
Q_ENUM(EAccountType)

enum class EChatEntryType
{	
	K_EChatEntryTypeInvalid = 0,
	K_EChatEntryTypeChatMsg = 1,
	K_EChatEntryTypeTyping = 2,
	K_EChatEntryTypeInviteGame = 3,
	K_EChatEntryTypeEmote = 4,
	K_EChatEntryTypeLeftConversation = 6,
	K_EChatEntryTypeEntered = 7,
	K_EChatEntryTypeWasKicked = 8,
	K_EChatEntryTypeWasBanned = 9,
	K_EChatEntryTypeDisconnected = 10,
	K_EChatEntryTypeHistoricalChat = 11,
	K_EChatEntryTypeLinkBlocked = 14,
 };
Q_ENUM(EChatEntryType)

enum class EChatRoomEnterResponse
{	
	K_EChatRoomEnterResponseSuccess = 1,
	K_EChatRoomEnterResponseDoesntExist = 2,
	K_EChatRoomEnterResponseNotAllowed = 3,
	K_EChatRoomEnterResponseFull = 4,
	K_EChatRoomEnterResponseError = 5,
	K_EChatRoomEnterResponseBanned = 6,
	K_EChatRoomEnterResponseLimited = 7,
	K_EChatRoomEnterResponseClanDisabled = 8,
	K_EChatRoomEnterResponseCommunityBan = 9,
	K_EChatRoomEnterResponseMemberBlockedYou = 10,
	K_EChatRoomEnterResponseYouBlockedMember = 11,
	K_EChatRoomEnterResponseRatelimitExceeded = 15,
 };
Q_ENUM(EChatRoomEnterResponse)

enum class EChatSteamIDInstanceFlags
{	
	K_EChatAccountInstanceMask = 4095,
	K_EChatInstanceFlagClan = 524288,
	K_EChatInstanceFlagLobby = 262144,
	K_EChatInstanceFlagMMSLobby = 131072,
 };
Q_ENUM(EChatSteamIDInstanceFlags)

enum class ENotificationPosition
{	
	K_EPositionInvalid = -1,
	K_EPositionTopLeft = 0,
	K_EPositionTopRight = 1,
	K_EPositionBottomLeft = 2,
	K_EPositionBottomRight = 3,
 };
Q_ENUM(ENotificationPosition)

enum class EBroadcastUploadResult
{	
	K_EBroadcastUploadResultNone = 0,
	K_EBroadcastUploadResultOK = 1,
	K_EBroadcastUploadResultInitFailed = 2,
	K_EBroadcastUploadResultFrameFailed = 3,
	K_EBroadcastUploadResultTimeout = 4,
	K_EBroadcastUploadResultBandwidthExceeded = 5,
	K_EBroadcastUploadResultLowFPS = 6,
	K_EBroadcastUploadResultMissingKeyFrames = 7,
	K_EBroadcastUploadResultNoConnection = 8,
	K_EBroadcastUploadResultRelayFailed = 9,
	K_EBroadcastUploadResultSettingsChanged = 10,
	K_EBroadcastUploadResultMissingAudio = 11,
	K_EBroadcastUploadResultTooFarBehind = 12,
	K_EBroadcastUploadResultTranscodeBehind = 13,
	K_EBroadcastUploadResultNotAllowedToPlay = 14,
	K_EBroadcastUploadResultBusy = 15,
	K_EBroadcastUploadResultBanned = 16,
	K_EBroadcastUploadResultAlreadyActive = 17,
	K_EBroadcastUploadResultForcedOff = 18,
	K_EBroadcastUploadResultAudioBehind = 19,
	K_EBroadcastUploadResultShutdown = 20,
	K_EBroadcastUploadResultDisconnect = 21,
	K_EBroadcastUploadResultVideoInitFailed = 22,
	K_EBroadcastUploadResultAudioInitFailed = 23,
 };
Q_ENUM(EBroadcastUploadResult)

enum class EMarketNotAllowedReasonFlags
{	
	K_EMarketNotAllowedReason_None = 0,
	K_EMarketNotAllowedReason_TemporaryFailure = 1,
	K_EMarketNotAllowedReason_AccountDisabled = 2,
	K_EMarketNotAllowedReason_AccountLockedDown = 4,
	K_EMarketNotAllowedReason_AccountLimited = 8,
	K_EMarketNotAllowedReason_TradeBanned = 16,
	K_EMarketNotAllowedReason_AccountNotTrusted = 32,
	K_EMarketNotAllowedReason_SteamGuardNotEnabled = 64,
	K_EMarketNotAllowedReason_SteamGuardOnlyRecentlyEnabled = 128,
	K_EMarketNotAllowedReason_RecentPasswordReset = 256,
	K_EMarketNotAllowedReason_NewPaymentMethod = 512,
	K_EMarketNotAllowedReason_InvalidCookie = 1024,
	K_EMarketNotAllowedReason_UsingNewDevice = 2048,
	K_EMarketNotAllowedReason_RecentSelfRefund = 4096,
	K_EMarketNotAllowedReason_NewPaymentMethodCannotBeVerified = 8192,
	K_EMarketNotAllowedReason_NoRecentPurchases = 16384,
	K_EMarketNotAllowedReason_AcceptedWalletGift = 32768,
 };
Q_ENUM(EMarketNotAllowedReasonFlags)

enum class EDurationControlProgress
{	
	K_EDurationControlProgress_Full = 0,
	K_EDurationControlProgress_Half = 1,
	K_EDurationControlProgress_None = 2,
	K_EDurationControl_ExitSoon_3h = 3,
	K_EDurationControl_ExitSoon_5h = 4,
	K_EDurationControl_ExitSoon_Night = 5,
 };
Q_ENUM(EDurationControlProgress)

enum class EDurationControlNotification
{	
	K_EDurationControlNotification_None = 0,
	K_EDurationControlNotification_1Hour = 1,
	K_EDurationControlNotification_3Hours = 2,
	K_EDurationControlNotification_HalfProgress = 3,
	K_EDurationControlNotification_NoProgress = 4,
	K_EDurationControlNotification_ExitSoon_3h = 5,
	K_EDurationControlNotification_ExitSoon_5h = 6,
	K_EDurationControlNotification_ExitSoon_Night = 7,
 };
Q_ENUM(EDurationControlNotification)

enum class EDurationControlOnlineState
{	
	K_EDurationControlOnlineState_Invalid = 0,
	K_EDurationControlOnlineState_Offline = 1,
	K_EDurationControlOnlineState_Online = 2,
	K_EDurationControlOnlineState_OnlineHighPri = 3,
 };
Q_ENUM(EDurationControlOnlineState)

enum class EGameSearchErrorCode_t
{	
	K_EGameSearchErrorCode_OK = 1,
	K_EGameSearchErrorCode_Failed_Search_Already_In_Progress = 2,
	K_EGameSearchErrorCode_Failed_No_Search_In_Progress = 3,
	K_EGameSearchErrorCode_Failed_Not_Lobby_Leader = 4,
	K_EGameSearchErrorCode_Failed_No_Host_Available = 5,
	K_EGameSearchErrorCode_Failed_Search_Params_Invalid = 6,
	K_EGameSearchErrorCode_Failed_Offline = 7,
	K_EGameSearchErrorCode_Failed_NotAuthorized = 8,
	K_EGameSearchErrorCode_Failed_Unknown_Error = 9,
 };
Q_ENUM(EGameSearchErrorCode_t)

enum class EPlayerResult_t
{	
	K_EPlayerResultFailedToConnect = 1,
	K_EPlayerResultAbandoned = 2,
	K_EPlayerResultKicked = 3,
	K_EPlayerResultIncomplete = 4,
	K_EPlayerResultCompleted = 5,
 };
Q_ENUM(EPlayerResult_t)

enum class ESteamIPv6ConnectivityProtocol
{	
	K_ESteamIPv6ConnectivityProtocol_Invalid = 0,
	K_ESteamIPv6ConnectivityProtocol_HTTP = 1,
	K_ESteamIPv6ConnectivityProtocol_UDP = 2,
 };
Q_ENUM(ESteamIPv6ConnectivityProtocol)

enum class ESteamIPv6ConnectivityState
{	
	K_ESteamIPv6ConnectivityState_Unknown = 0,
	K_ESteamIPv6ConnectivityState_Good = 1,
	K_ESteamIPv6ConnectivityState_Bad = 2,
 };
Q_ENUM(ESteamIPv6ConnectivityState)

enum class EFriendRelationship
{	
	K_EFriendRelationshipNone = 0,
	K_EFriendRelationshipBlocked = 1,
	K_EFriendRelationshipRequestRecipient = 2,
	K_EFriendRelationshipFriend = 3,
	K_EFriendRelationshipRequestInitiator = 4,
	K_EFriendRelationshipIgnored = 5,
	K_EFriendRelationshipIgnoredFriend = 6,
	K_EFriendRelationshipSuggested_DEPRECATED = 7,
	K_EFriendRelationshipMax = 8,
 };
Q_ENUM(EFriendRelationship)

enum class EPersonaState
{	
	K_EPersonaStateOffline = 0,
	K_EPersonaStateOnline = 1,
	K_EPersonaStateBusy = 2,
	K_EPersonaStateAway = 3,
	K_EPersonaStateSnooze = 4,
	K_EPersonaStateLookingToTrade = 5,
	K_EPersonaStateLookingToPlay = 6,
	K_EPersonaStateInvisible = 7,
	K_EPersonaStateMax = 8,
 };
Q_ENUM(EPersonaState)

enum class EFriendFlags
{	
	K_EFriendFlagNone = 0,
	K_EFriendFlagBlocked = 1,
	K_EFriendFlagFriendshipRequested = 2,
	K_EFriendFlagImmediate = 4,
	K_EFriendFlagClanMember = 8,
	K_EFriendFlagOnGameServer = 16,
	K_EFriendFlagRequestingFriendship = 128,
	K_EFriendFlagRequestingInfo = 256,
	K_EFriendFlagIgnored = 512,
	K_EFriendFlagIgnoredFriend = 1024,
	K_EFriendFlagChatMember = 4096,
	K_EFriendFlagAll = 65535,
 };
Q_ENUM(EFriendFlags)

enum class EUserRestriction
{	
	K_nUserRestrictionNone = 0,
	K_nUserRestrictionUnknown = 1,
	K_nUserRestrictionAnyChat = 2,
	K_nUserRestrictionVoiceChat = 4,
	K_nUserRestrictionGroupChat = 8,
	K_nUserRestrictionRating = 16,
	K_nUserRestrictionGameInvites = 32,
	K_nUserRestrictionTrading = 64,
 };
Q_ENUM(EUserRestriction)

enum class EOverlayToStoreFlag
{	
	K_EOverlayToStoreFlag_None = 0,
	K_EOverlayToStoreFlag_AddToCart = 1,
	K_EOverlayToStoreFlag_AddToCartAndShow = 2,
 };
Q_ENUM(EOverlayToStoreFlag)

enum class EActivateGameOverlayToWebPageMode
{	
	K_EActivateGameOverlayToWebPageMode_Default = 0,
	K_EActivateGameOverlayToWebPageMode_Modal = 1,
 };
Q_ENUM(EActivateGameOverlayToWebPageMode)

enum class ECommunityProfileItemType
{	
	K_ECommunityProfileItemType_AnimatedAvatar = 0,
	K_ECommunityProfileItemType_AvatarFrame = 1,
	K_ECommunityProfileItemType_ProfileModifier = 2,
	K_ECommunityProfileItemType_ProfileBackground = 3,
	K_ECommunityProfileItemType_MiniProfileBackground = 4,
 };
Q_ENUM(ECommunityProfileItemType)

enum class ECommunityProfileItemProperty
{	
	K_ECommunityProfileItemProperty_ImageSmall = 0,
	K_ECommunityProfileItemProperty_ImageLarge = 1,
	K_ECommunityProfileItemProperty_InternalName = 2,
	K_ECommunityProfileItemProperty_Title = 3,
	K_ECommunityProfileItemProperty_Description = 4,
	K_ECommunityProfileItemProperty_AppID = 5,
	K_ECommunityProfileItemProperty_TypeID = 6,
	K_ECommunityProfileItemProperty_Class = 7,
	K_ECommunityProfileItemProperty_MovieWebM = 8,
	K_ECommunityProfileItemProperty_MovieMP4 = 9,
	K_ECommunityProfileItemProperty_MovieWebMSmall = 10,
	K_ECommunityProfileItemProperty_MovieMP4Small = 11,
 };
Q_ENUM(ECommunityProfileItemProperty)

enum class EPersonaChange
{	
	K_EPersonaChangeName = 1,
	K_EPersonaChangeStatus = 2,
	K_EPersonaChangeComeOnline = 4,
	K_EPersonaChangeGoneOffline = 8,
	K_EPersonaChangeGamePlayed = 16,
	K_EPersonaChangeGameServer = 32,
	K_EPersonaChangeAvatar = 64,
	K_EPersonaChangeJoinedSource = 128,
	K_EPersonaChangeLeftSource = 256,
	K_EPersonaChangeRelationshipChanged = 512,
	K_EPersonaChangeNameFirstSet = 1024,
	K_EPersonaChangeBroadcast = 2048,
	K_EPersonaChangeNickname = 4096,
	K_EPersonaChangeSteamLevel = 8192,
	K_EPersonaChangeRichPresence = 16384,
 };
Q_ENUM(EPersonaChange)

enum class ESteamAPICallFailure
{	
	K_ESteamAPICallFailureNone = -1,
	K_ESteamAPICallFailureSteamGone = 0,
	K_ESteamAPICallFailureNetworkFailure = 1,
	K_ESteamAPICallFailureInvalidHandle = 2,
	K_ESteamAPICallFailureMismatchedCallback = 3,
 };
Q_ENUM(ESteamAPICallFailure)

enum class EGamepadTextInputMode
{	
	K_EGamepadTextInputModeNormal = 0,
	K_EGamepadTextInputModePassword = 1,
 };
Q_ENUM(EGamepadTextInputMode)

enum class EGamepadTextInputLineMode
{	
	K_EGamepadTextInputLineModeSingleLine = 0,
	K_EGamepadTextInputLineModeMultipleLines = 1,
 };
Q_ENUM(EGamepadTextInputLineMode)

enum class EFloatingGamepadTextInputMode
{	
	K_EFloatingGamepadTextInputModeModeSingleLine = 0,
	K_EFloatingGamepadTextInputModeModeMultipleLines = 1,
	K_EFloatingGamepadTextInputModeModeEmail = 2,
	K_EFloatingGamepadTextInputModeModeNumeric = 3,
 };
Q_ENUM(EFloatingGamepadTextInputMode)

enum class ETextFilteringContext
{	
	K_ETextFilteringContextUnknown = 0,
	K_ETextFilteringContextGameContent = 1,
	K_ETextFilteringContextChat = 2,
	K_ETextFilteringContextName = 3,
 };
Q_ENUM(ETextFilteringContext)

enum class ECheckFileSignature
{	
	K_ECheckFileSignatureInvalidSignature = 0,
	K_ECheckFileSignatureValidSignature = 1,
	K_ECheckFileSignatureFileNotFound = 2,
	K_ECheckFileSignatureNoSignaturesFoundForThisApp = 3,
	K_ECheckFileSignatureNoSignaturesFoundForThisFile = 4,
 };
Q_ENUM(ECheckFileSignature)

enum class EMatchMakingServerResponse
{	
	EServerResponded = 0,
	EServerFailedToRespond = 1,
	ENoServersListedOnMasterServer = 2,
 };
Q_ENUM(EMatchMakingServerResponse)

enum class ELobbyType
{	
	K_ELobbyTypePrivate = 0,
	K_ELobbyTypeFriendsOnly = 1,
	K_ELobbyTypePublic = 2,
	K_ELobbyTypeInvisible = 3,
	K_ELobbyTypePrivateUnique = 4,
 };
Q_ENUM(ELobbyType)

enum class ELobbyComparison
{	
	K_ELobbyComparisonEqualToOrLessThan = -2,
	K_ELobbyComparisonLessThan = -1,
	K_ELobbyComparisonEqual = 0,
	K_ELobbyComparisonGreaterThan = 1,
	K_ELobbyComparisonEqualToOrGreaterThan = 2,
	K_ELobbyComparisonNotEqual = 3,
 };
Q_ENUM(ELobbyComparison)

enum class ELobbyDistanceFilter
{	
	K_ELobbyDistanceFilterClose = 0,
	K_ELobbyDistanceFilterDefault = 1,
	K_ELobbyDistanceFilterFar = 2,
	K_ELobbyDistanceFilterWorldwide = 3,
 };
Q_ENUM(ELobbyDistanceFilter)

enum class EChatMemberStateChange
{	
	K_EChatMemberStateChangeEntered = 1,
	K_EChatMemberStateChangeLeft = 2,
	K_EChatMemberStateChangeDisconnected = 4,
	K_EChatMemberStateChangeKicked = 8,
	K_EChatMemberStateChangeBanned = 16,
 };
Q_ENUM(EChatMemberStateChange)

enum class ESteamPartyBeaconLocationType
{	
	K_ESteamPartyBeaconLocationType_Invalid = 0,
	K_ESteamPartyBeaconLocationType_ChatGroup = 1,
	K_ESteamPartyBeaconLocationType_Max = 2,
 };
Q_ENUM(ESteamPartyBeaconLocationType)

enum class ESteamPartyBeaconLocationData
{	
	K_ESteamPartyBeaconLocationDataInvalid = 0,
	K_ESteamPartyBeaconLocationDataName = 1,
	K_ESteamPartyBeaconLocationDataIconURLSmall = 2,
	K_ESteamPartyBeaconLocationDataIconURLMedium = 3,
	K_ESteamPartyBeaconLocationDataIconURLLarge = 4,
 };
Q_ENUM(ESteamPartyBeaconLocationData)

enum class ERemoteStoragePlatform
{	
	K_ERemoteStoragePlatformNone = 0,
	K_ERemoteStoragePlatformWindows = 1,
	K_ERemoteStoragePlatformOSX = 2,
	K_ERemoteStoragePlatformPS3 = 4,
	K_ERemoteStoragePlatformLinux = 8,
	K_ERemoteStoragePlatformSwitch = 16,
	K_ERemoteStoragePlatformAndroid = 32,
	K_ERemoteStoragePlatformIOS = 64,
	K_ERemoteStoragePlatformAll = -1,
 };
Q_ENUM(ERemoteStoragePlatform)

enum class ERemoteStoragePublishedFileVisibility
{	
	K_ERemoteStoragePublishedFileVisibilityPublic = 0,
	K_ERemoteStoragePublishedFileVisibilityFriendsOnly = 1,
	K_ERemoteStoragePublishedFileVisibilityPrivate = 2,
	K_ERemoteStoragePublishedFileVisibilityUnlisted = 3,
 };
Q_ENUM(ERemoteStoragePublishedFileVisibility)

enum class EWorkshopFileType
{	
	K_EWorkshopFileTypeFirst = 0,
	K_EWorkshopFileTypeCommunity = 0,
	K_EWorkshopFileTypeMicrotransaction = 1,
	K_EWorkshopFileTypeCollection = 2,
	K_EWorkshopFileTypeArt = 3,
	K_EWorkshopFileTypeVideo = 4,
	K_EWorkshopFileTypeScreenshot = 5,
	K_EWorkshopFileTypeGame = 6,
	K_EWorkshopFileTypeSoftware = 7,
	K_EWorkshopFileTypeConcept = 8,
	K_EWorkshopFileTypeWebGuide = 9,
	K_EWorkshopFileTypeIntegratedGuide = 10,
	K_EWorkshopFileTypeMerch = 11,
	K_EWorkshopFileTypeControllerBinding = 12,
	K_EWorkshopFileTypeSteamworksAccessInvite = 13,
	K_EWorkshopFileTypeSteamVideo = 14,
	K_EWorkshopFileTypeGameManagedItem = 15,
	K_EWorkshopFileTypeMax = 16,
 };
Q_ENUM(EWorkshopFileType)

enum class EWorkshopVote
{	
	K_EWorkshopVoteUnvoted = 0,
	K_EWorkshopVoteFor = 1,
	K_EWorkshopVoteAgainst = 2,
	K_EWorkshopVoteLater = 3,
 };
Q_ENUM(EWorkshopVote)

enum class EWorkshopFileAction
{	
	K_EWorkshopFileActionPlayed = 0,
	K_EWorkshopFileActionCompleted = 1,
 };
Q_ENUM(EWorkshopFileAction)

enum class EWorkshopEnumerationType
{	
	K_EWorkshopEnumerationTypeRankedByVote = 0,
	K_EWorkshopEnumerationTypeRecent = 1,
	K_EWorkshopEnumerationTypeTrending = 2,
	K_EWorkshopEnumerationTypeFavoritesOfFriends = 3,
	K_EWorkshopEnumerationTypeVotedByFriends = 4,
	K_EWorkshopEnumerationTypeContentByFriends = 5,
	K_EWorkshopEnumerationTypeRecentFromFollowedUsers = 6,
 };
Q_ENUM(EWorkshopEnumerationType)

enum class EWorkshopVideoProvider
{	
	K_EWorkshopVideoProviderNone = 0,
	K_EWorkshopVideoProviderYoutube = 1,
 };
Q_ENUM(EWorkshopVideoProvider)

enum class EUGCReadAction
{	
	K_EUGCRead_ContinueReadingUntilFinished = 0,
	K_EUGCRead_ContinueReading = 1,
	K_EUGCRead_Close = 2,
 };
Q_ENUM(EUGCReadAction)

enum class ERemoteStorageLocalFileChange
{	
	K_ERemoteStorageLocalFileChange_Invalid = 0,
	K_ERemoteStorageLocalFileChange_FileUpdated = 1,
	K_ERemoteStorageLocalFileChange_FileDeleted = 2,
 };
Q_ENUM(ERemoteStorageLocalFileChange)

enum class ERemoteStorageFilePathType
{	
	K_ERemoteStorageFilePathType_Invalid = 0,
	K_ERemoteStorageFilePathType_Absolute = 1,
	K_ERemoteStorageFilePathType_APIFilename = 2,
 };
Q_ENUM(ERemoteStorageFilePathType)

enum class ELeaderboardDataRequest
{	
	K_ELeaderboardDataRequestGlobal = 0,
	K_ELeaderboardDataRequestGlobalAroundUser = 1,
	K_ELeaderboardDataRequestFriends = 2,
	K_ELeaderboardDataRequestUsers = 3,
 };
Q_ENUM(ELeaderboardDataRequest)

enum class ELeaderboardSortMethod
{	
	K_ELeaderboardSortMethodNone = 0,
	K_ELeaderboardSortMethodAscending = 1,
	K_ELeaderboardSortMethodDescending = 2,
 };
Q_ENUM(ELeaderboardSortMethod)

enum class ELeaderboardDisplayType
{	
	K_ELeaderboardDisplayTypeNone = 0,
	K_ELeaderboardDisplayTypeNumeric = 1,
	K_ELeaderboardDisplayTypeTimeSeconds = 2,
	K_ELeaderboardDisplayTypeTimeMilliSeconds = 3,
 };
Q_ENUM(ELeaderboardDisplayType)

enum class ELeaderboardUploadScoreMethod
{	
	K_ELeaderboardUploadScoreMethodNone = 0,
	K_ELeaderboardUploadScoreMethodKeepBest = 1,
	K_ELeaderboardUploadScoreMethodForceUpdate = 2,
 };
Q_ENUM(ELeaderboardUploadScoreMethod)

enum class EP2PSessionError
{	
	K_EP2PSessionErrorNone = 0,
	K_EP2PSessionErrorNoRightsToApp = 2,
	K_EP2PSessionErrorTimeout = 4,
	K_EP2PSessionErrorNotRunningApp_DELETED = 1,
	K_EP2PSessionErrorDestinationNotLoggedIn_DELETED = 3,
	K_EP2PSessionErrorMax = 5,
 };
Q_ENUM(EP2PSessionError)

enum class EP2PSend
{	
	K_EP2PSendUnreliable = 0,
	K_EP2PSendUnreliableNoDelay = 1,
	K_EP2PSendReliable = 2,
	K_EP2PSendReliableWithBuffering = 3,
 };
Q_ENUM(EP2PSend)

enum class ESNetSocketState
{	
	K_ESNetSocketStateInvalid = 0,
	K_ESNetSocketStateConnected = 1,
	K_ESNetSocketStateInitiated = 10,
	K_ESNetSocketStateLocalCandidatesFound = 11,
	K_ESNetSocketStateReceivedRemoteCandidates = 12,
	K_ESNetSocketStateChallengeHandshake = 15,
	K_ESNetSocketStateDisconnecting = 21,
	K_ESNetSocketStateLocalDisconnect = 22,
	K_ESNetSocketStateTimeoutDuringConnect = 23,
	K_ESNetSocketStateRemoteEndDisconnected = 24,
	K_ESNetSocketStateConnectionBroken = 25,
 };
Q_ENUM(ESNetSocketState)

enum class ESNetSocketConnectionType
{	
	K_ESNetSocketConnectionTypeNotConnected = 0,
	K_ESNetSocketConnectionTypeUDP = 1,
	K_ESNetSocketConnectionTypeUDPRelay = 2,
 };
Q_ENUM(ESNetSocketConnectionType)

enum class EVRScreenshotType
{	
	K_EVRScreenshotType_None = 0,
	K_EVRScreenshotType_Mono = 1,
	K_EVRScreenshotType_Stereo = 2,
	K_EVRScreenshotType_MonoCubemap = 3,
	K_EVRScreenshotType_MonoPanorama = 4,
	K_EVRScreenshotType_StereoPanorama = 5,
 };
Q_ENUM(EVRScreenshotType)

enum class AudioPlayback_Status
{	
	AudioPlayback_Undefined = 0,
	AudioPlayback_Playing = 1,
	AudioPlayback_Paused = 2,
	AudioPlayback_Idle = 3,
 };
Q_ENUM(AudioPlayback_Status)

enum class EHTTPMethod
{	
	K_EHTTPMethodInvalid = 0,
	K_EHTTPMethodGET = 1,
	K_EHTTPMethodHEAD = 2,
	K_EHTTPMethodPOST = 3,
	K_EHTTPMethodPUT = 4,
	K_EHTTPMethodDELETE = 5,
	K_EHTTPMethodOPTIONS = 6,
	K_EHTTPMethodPATCH = 7,
 };
Q_ENUM(EHTTPMethod)

enum class EHTTPStatusCode
{	
	K_EHTTPStatusCodeInvalid = 0,
	K_EHTTPStatusCode100Continue = 100,
	K_EHTTPStatusCode101SwitchingProtocols = 101,
	K_EHTTPStatusCode200OK = 200,
	K_EHTTPStatusCode201Created = 201,
	K_EHTTPStatusCode202Accepted = 202,
	K_EHTTPStatusCode203NonAuthoritative = 203,
	K_EHTTPStatusCode204NoContent = 204,
	K_EHTTPStatusCode205ResetContent = 205,
	K_EHTTPStatusCode206PartialContent = 206,
	K_EHTTPStatusCode300MultipleChoices = 300,
	K_EHTTPStatusCode301MovedPermanently = 301,
	K_EHTTPStatusCode302Found = 302,
	K_EHTTPStatusCode303SeeOther = 303,
	K_EHTTPStatusCode304NotModified = 304,
	K_EHTTPStatusCode305UseProxy = 305,
	K_EHTTPStatusCode307TemporaryRedirect = 307,
	K_EHTTPStatusCode308PermanentRedirect = 308,
	K_EHTTPStatusCode400BadRequest = 400,
	K_EHTTPStatusCode401Unauthorized = 401,
	K_EHTTPStatusCode402PaymentRequired = 402,
	K_EHTTPStatusCode403Forbidden = 403,
	K_EHTTPStatusCode404NotFound = 404,
	K_EHTTPStatusCode405MethodNotAllowed = 405,
	K_EHTTPStatusCode406NotAcceptable = 406,
	K_EHTTPStatusCode407ProxyAuthRequired = 407,
	K_EHTTPStatusCode408RequestTimeout = 408,
	K_EHTTPStatusCode409Conflict = 409,
	K_EHTTPStatusCode410Gone = 410,
	K_EHTTPStatusCode411LengthRequired = 411,
	K_EHTTPStatusCode412PreconditionFailed = 412,
	K_EHTTPStatusCode413RequestEntityTooLarge = 413,
	K_EHTTPStatusCode414RequestURITooLong = 414,
	K_EHTTPStatusCode415UnsupportedMediaType = 415,
	K_EHTTPStatusCode416RequestedRangeNotSatisfiable = 416,
	K_EHTTPStatusCode417ExpectationFailed = 417,
	K_EHTTPStatusCode4xxUnknown = 418,
	K_EHTTPStatusCode429TooManyRequests = 429,
	K_EHTTPStatusCode444ConnectionClosed = 444,
	K_EHTTPStatusCode500InternalServerError = 500,
	K_EHTTPStatusCode501NotImplemented = 501,
	K_EHTTPStatusCode502BadGateway = 502,
	K_EHTTPStatusCode503ServiceUnavailable = 503,
	K_EHTTPStatusCode504GatewayTimeout = 504,
	K_EHTTPStatusCode505HTTPVersionNotSupported = 505,
	K_EHTTPStatusCode5xxUnknown = 599,
 };
Q_ENUM(EHTTPStatusCode)

enum class EInputSourceMode
{	
	K_EInputSourceMode_None = 0,
	K_EInputSourceMode_Dpad = 1,
	K_EInputSourceMode_Buttons = 2,
	K_EInputSourceMode_FourButtons = 3,
	K_EInputSourceMode_AbsoluteMouse = 4,
	K_EInputSourceMode_RelativeMouse = 5,
	K_EInputSourceMode_JoystickMove = 6,
	K_EInputSourceMode_JoystickMouse = 7,
	K_EInputSourceMode_JoystickCamera = 8,
	K_EInputSourceMode_ScrollWheel = 9,
	K_EInputSourceMode_Trigger = 10,
	K_EInputSourceMode_TouchMenu = 11,
	K_EInputSourceMode_MouseJoystick = 12,
	K_EInputSourceMode_MouseRegion = 13,
	K_EInputSourceMode_RadialMenu = 14,
	K_EInputSourceMode_SingleButton = 15,
	K_EInputSourceMode_Switches = 16,
 };
Q_ENUM(EInputSourceMode)

enum class EInputActionOrigin
{	
	K_EInputActionOrigin_None = 0,
	K_EInputActionOrigin_SteamController_A = 1,
	K_EInputActionOrigin_SteamController_B = 2,
	K_EInputActionOrigin_SteamController_X = 3,
	K_EInputActionOrigin_SteamController_Y = 4,
	K_EInputActionOrigin_SteamController_LeftBumper = 5,
	K_EInputActionOrigin_SteamController_RightBumper = 6,
	K_EInputActionOrigin_SteamController_LeftGrip = 7,
	K_EInputActionOrigin_SteamController_RightGrip = 8,
	K_EInputActionOrigin_SteamController_Start = 9,
	K_EInputActionOrigin_SteamController_Back = 10,
	K_EInputActionOrigin_SteamController_LeftPad_Touch = 11,
	K_EInputActionOrigin_SteamController_LeftPad_Swipe = 12,
	K_EInputActionOrigin_SteamController_LeftPad_Click = 13,
	K_EInputActionOrigin_SteamController_LeftPad_DPadNorth = 14,
	K_EInputActionOrigin_SteamController_LeftPad_DPadSouth = 15,
	K_EInputActionOrigin_SteamController_LeftPad_DPadWest = 16,
	K_EInputActionOrigin_SteamController_LeftPad_DPadEast = 17,
	K_EInputActionOrigin_SteamController_RightPad_Touch = 18,
	K_EInputActionOrigin_SteamController_RightPad_Swipe = 19,
	K_EInputActionOrigin_SteamController_RightPad_Click = 20,
	K_EInputActionOrigin_SteamController_RightPad_DPadNorth = 21,
	K_EInputActionOrigin_SteamController_RightPad_DPadSouth = 22,
	K_EInputActionOrigin_SteamController_RightPad_DPadWest = 23,
	K_EInputActionOrigin_SteamController_RightPad_DPadEast = 24,
	K_EInputActionOrigin_SteamController_LeftTrigger_Pull = 25,
	K_EInputActionOrigin_SteamController_LeftTrigger_Click = 26,
	K_EInputActionOrigin_SteamController_RightTrigger_Pull = 27,
	K_EInputActionOrigin_SteamController_RightTrigger_Click = 28,
	K_EInputActionOrigin_SteamController_LeftStick_Move = 29,
	K_EInputActionOrigin_SteamController_LeftStick_Click = 30,
	K_EInputActionOrigin_SteamController_LeftStick_DPadNorth = 31,
	K_EInputActionOrigin_SteamController_LeftStick_DPadSouth = 32,
	K_EInputActionOrigin_SteamController_LeftStick_DPadWest = 33,
	K_EInputActionOrigin_SteamController_LeftStick_DPadEast = 34,
	K_EInputActionOrigin_SteamController_Gyro_Move = 35,
	K_EInputActionOrigin_SteamController_Gyro_Pitch = 36,
	K_EInputActionOrigin_SteamController_Gyro_Yaw = 37,
	K_EInputActionOrigin_SteamController_Gyro_Roll = 38,
	K_EInputActionOrigin_SteamController_Reserved0 = 39,
	K_EInputActionOrigin_SteamController_Reserved1 = 40,
	K_EInputActionOrigin_SteamController_Reserved2 = 41,
	K_EInputActionOrigin_SteamController_Reserved3 = 42,
	K_EInputActionOrigin_SteamController_Reserved4 = 43,
	K_EInputActionOrigin_SteamController_Reserved5 = 44,
	K_EInputActionOrigin_SteamController_Reserved6 = 45,
	K_EInputActionOrigin_SteamController_Reserved7 = 46,
	K_EInputActionOrigin_SteamController_Reserved8 = 47,
	K_EInputActionOrigin_SteamController_Reserved9 = 48,
	K_EInputActionOrigin_SteamController_Reserved10 = 49,
	K_EInputActionOrigin_PS4_X = 50,
	K_EInputActionOrigin_PS4_Circle = 51,
	K_EInputActionOrigin_PS4_Triangle = 52,
	K_EInputActionOrigin_PS4_Square = 53,
	K_EInputActionOrigin_PS4_LeftBumper = 54,
	K_EInputActionOrigin_PS4_RightBumper = 55,
	K_EInputActionOrigin_PS4_Options = 56,
	K_EInputActionOrigin_PS4_Share = 57,
	K_EInputActionOrigin_PS4_LeftPad_Touch = 58,
	K_EInputActionOrigin_PS4_LeftPad_Swipe = 59,
	K_EInputActionOrigin_PS4_LeftPad_Click = 60,
	K_EInputActionOrigin_PS4_LeftPad_DPadNorth = 61,
	K_EInputActionOrigin_PS4_LeftPad_DPadSouth = 62,
	K_EInputActionOrigin_PS4_LeftPad_DPadWest = 63,
	K_EInputActionOrigin_PS4_LeftPad_DPadEast = 64,
	K_EInputActionOrigin_PS4_RightPad_Touch = 65,
	K_EInputActionOrigin_PS4_RightPad_Swipe = 66,
	K_EInputActionOrigin_PS4_RightPad_Click = 67,
	K_EInputActionOrigin_PS4_RightPad_DPadNorth = 68,
	K_EInputActionOrigin_PS4_RightPad_DPadSouth = 69,
	K_EInputActionOrigin_PS4_RightPad_DPadWest = 70,
	K_EInputActionOrigin_PS4_RightPad_DPadEast = 71,
	K_EInputActionOrigin_PS4_CenterPad_Touch = 72,
	K_EInputActionOrigin_PS4_CenterPad_Swipe = 73,
	K_EInputActionOrigin_PS4_CenterPad_Click = 74,
	K_EInputActionOrigin_PS4_CenterPad_DPadNorth = 75,
	K_EInputActionOrigin_PS4_CenterPad_DPadSouth = 76,
	K_EInputActionOrigin_PS4_CenterPad_DPadWest = 77,
	K_EInputActionOrigin_PS4_CenterPad_DPadEast = 78,
	K_EInputActionOrigin_PS4_LeftTrigger_Pull = 79,
	K_EInputActionOrigin_PS4_LeftTrigger_Click = 80,
	K_EInputActionOrigin_PS4_RightTrigger_Pull = 81,
	K_EInputActionOrigin_PS4_RightTrigger_Click = 82,
	K_EInputActionOrigin_PS4_LeftStick_Move = 83,
	K_EInputActionOrigin_PS4_LeftStick_Click = 84,
	K_EInputActionOrigin_PS4_LeftStick_DPadNorth = 85,
	K_EInputActionOrigin_PS4_LeftStick_DPadSouth = 86,
	K_EInputActionOrigin_PS4_LeftStick_DPadWest = 87,
	K_EInputActionOrigin_PS4_LeftStick_DPadEast = 88,
	K_EInputActionOrigin_PS4_RightStick_Move = 89,
	K_EInputActionOrigin_PS4_RightStick_Click = 90,
	K_EInputActionOrigin_PS4_RightStick_DPadNorth = 91,
	K_EInputActionOrigin_PS4_RightStick_DPadSouth = 92,
	K_EInputActionOrigin_PS4_RightStick_DPadWest = 93,
	K_EInputActionOrigin_PS4_RightStick_DPadEast = 94,
	K_EInputActionOrigin_PS4_DPad_North = 95,
	K_EInputActionOrigin_PS4_DPad_South = 96,
	K_EInputActionOrigin_PS4_DPad_West = 97,
	K_EInputActionOrigin_PS4_DPad_East = 98,
	K_EInputActionOrigin_PS4_Gyro_Move = 99,
	K_EInputActionOrigin_PS4_Gyro_Pitch = 100,
	K_EInputActionOrigin_PS4_Gyro_Yaw = 101,
	K_EInputActionOrigin_PS4_Gyro_Roll = 102,
	K_EInputActionOrigin_PS4_DPad_Move = 103,
	K_EInputActionOrigin_PS4_Reserved1 = 104,
	K_EInputActionOrigin_PS4_Reserved2 = 105,
	K_EInputActionOrigin_PS4_Reserved3 = 106,
	K_EInputActionOrigin_PS4_Reserved4 = 107,
	K_EInputActionOrigin_PS4_Reserved5 = 108,
	K_EInputActionOrigin_PS4_Reserved6 = 109,
	K_EInputActionOrigin_PS4_Reserved7 = 110,
	K_EInputActionOrigin_PS4_Reserved8 = 111,
	K_EInputActionOrigin_PS4_Reserved9 = 112,
	K_EInputActionOrigin_PS4_Reserved10 = 113,
	K_EInputActionOrigin_XBoxOne_A = 114,
	K_EInputActionOrigin_XBoxOne_B = 115,
	K_EInputActionOrigin_XBoxOne_X = 116,
	K_EInputActionOrigin_XBoxOne_Y = 117,
	K_EInputActionOrigin_XBoxOne_LeftBumper = 118,
	K_EInputActionOrigin_XBoxOne_RightBumper = 119,
	K_EInputActionOrigin_XBoxOne_Menu = 120,
	K_EInputActionOrigin_XBoxOne_View = 121,
	K_EInputActionOrigin_XBoxOne_LeftTrigger_Pull = 122,
	K_EInputActionOrigin_XBoxOne_LeftTrigger_Click = 123,
	K_EInputActionOrigin_XBoxOne_RightTrigger_Pull = 124,
	K_EInputActionOrigin_XBoxOne_RightTrigger_Click = 125,
	K_EInputActionOrigin_XBoxOne_LeftStick_Move = 126,
	K_EInputActionOrigin_XBoxOne_LeftStick_Click = 127,
	K_EInputActionOrigin_XBoxOne_LeftStick_DPadNorth = 128,
	K_EInputActionOrigin_XBoxOne_LeftStick_DPadSouth = 129,
	K_EInputActionOrigin_XBoxOne_LeftStick_DPadWest = 130,
	K_EInputActionOrigin_XBoxOne_LeftStick_DPadEast = 131,
	K_EInputActionOrigin_XBoxOne_RightStick_Move = 132,
	K_EInputActionOrigin_XBoxOne_RightStick_Click = 133,
	K_EInputActionOrigin_XBoxOne_RightStick_DPadNorth = 134,
	K_EInputActionOrigin_XBoxOne_RightStick_DPadSouth = 135,
	K_EInputActionOrigin_XBoxOne_RightStick_DPadWest = 136,
	K_EInputActionOrigin_XBoxOne_RightStick_DPadEast = 137,
	K_EInputActionOrigin_XBoxOne_DPad_North = 138,
	K_EInputActionOrigin_XBoxOne_DPad_South = 139,
	K_EInputActionOrigin_XBoxOne_DPad_West = 140,
	K_EInputActionOrigin_XBoxOne_DPad_East = 141,
	K_EInputActionOrigin_XBoxOne_DPad_Move = 142,
	K_EInputActionOrigin_XBoxOne_LeftGrip_Lower = 143,
	K_EInputActionOrigin_XBoxOne_LeftGrip_Upper = 144,
	K_EInputActionOrigin_XBoxOne_RightGrip_Lower = 145,
	K_EInputActionOrigin_XBoxOne_RightGrip_Upper = 146,
	K_EInputActionOrigin_XBoxOne_Share = 147,
	K_EInputActionOrigin_XBoxOne_Reserved6 = 148,
	K_EInputActionOrigin_XBoxOne_Reserved7 = 149,
	K_EInputActionOrigin_XBoxOne_Reserved8 = 150,
	K_EInputActionOrigin_XBoxOne_Reserved9 = 151,
	K_EInputActionOrigin_XBoxOne_Reserved10 = 152,
	K_EInputActionOrigin_XBox360_A = 153,
	K_EInputActionOrigin_XBox360_B = 154,
	K_EInputActionOrigin_XBox360_X = 155,
	K_EInputActionOrigin_XBox360_Y = 156,
	K_EInputActionOrigin_XBox360_LeftBumper = 157,
	K_EInputActionOrigin_XBox360_RightBumper = 158,
	K_EInputActionOrigin_XBox360_Start = 159,
	K_EInputActionOrigin_XBox360_Back = 160,
	K_EInputActionOrigin_XBox360_LeftTrigger_Pull = 161,
	K_EInputActionOrigin_XBox360_LeftTrigger_Click = 162,
	K_EInputActionOrigin_XBox360_RightTrigger_Pull = 163,
	K_EInputActionOrigin_XBox360_RightTrigger_Click = 164,
	K_EInputActionOrigin_XBox360_LeftStick_Move = 165,
	K_EInputActionOrigin_XBox360_LeftStick_Click = 166,
	K_EInputActionOrigin_XBox360_LeftStick_DPadNorth = 167,
	K_EInputActionOrigin_XBox360_LeftStick_DPadSouth = 168,
	K_EInputActionOrigin_XBox360_LeftStick_DPadWest = 169,
	K_EInputActionOrigin_XBox360_LeftStick_DPadEast = 170,
	K_EInputActionOrigin_XBox360_RightStick_Move = 171,
	K_EInputActionOrigin_XBox360_RightStick_Click = 172,
	K_EInputActionOrigin_XBox360_RightStick_DPadNorth = 173,
	K_EInputActionOrigin_XBox360_RightStick_DPadSouth = 174,
	K_EInputActionOrigin_XBox360_RightStick_DPadWest = 175,
	K_EInputActionOrigin_XBox360_RightStick_DPadEast = 176,
	K_EInputActionOrigin_XBox360_DPad_North = 177,
	K_EInputActionOrigin_XBox360_DPad_South = 178,
	K_EInputActionOrigin_XBox360_DPad_West = 179,
	K_EInputActionOrigin_XBox360_DPad_East = 180,
	K_EInputActionOrigin_XBox360_DPad_Move = 181,
	K_EInputActionOrigin_XBox360_Reserved1 = 182,
	K_EInputActionOrigin_XBox360_Reserved2 = 183,
	K_EInputActionOrigin_XBox360_Reserved3 = 184,
	K_EInputActionOrigin_XBox360_Reserved4 = 185,
	K_EInputActionOrigin_XBox360_Reserved5 = 186,
	K_EInputActionOrigin_XBox360_Reserved6 = 187,
	K_EInputActionOrigin_XBox360_Reserved7 = 188,
	K_EInputActionOrigin_XBox360_Reserved8 = 189,
	K_EInputActionOrigin_XBox360_Reserved9 = 190,
	K_EInputActionOrigin_XBox360_Reserved10 = 191,
	K_EInputActionOrigin_Switch_A = 192,
	K_EInputActionOrigin_Switch_B = 193,
	K_EInputActionOrigin_Switch_X = 194,
	K_EInputActionOrigin_Switch_Y = 195,
	K_EInputActionOrigin_Switch_LeftBumper = 196,
	K_EInputActionOrigin_Switch_RightBumper = 197,
	K_EInputActionOrigin_Switch_Plus = 198,
	K_EInputActionOrigin_Switch_Minus = 199,
	K_EInputActionOrigin_Switch_Capture = 200,
	K_EInputActionOrigin_Switch_LeftTrigger_Pull = 201,
	K_EInputActionOrigin_Switch_LeftTrigger_Click = 202,
	K_EInputActionOrigin_Switch_RightTrigger_Pull = 203,
	K_EInputActionOrigin_Switch_RightTrigger_Click = 204,
	K_EInputActionOrigin_Switch_LeftStick_Move = 205,
	K_EInputActionOrigin_Switch_LeftStick_Click = 206,
	K_EInputActionOrigin_Switch_LeftStick_DPadNorth = 207,
	K_EInputActionOrigin_Switch_LeftStick_DPadSouth = 208,
	K_EInputActionOrigin_Switch_LeftStick_DPadWest = 209,
	K_EInputActionOrigin_Switch_LeftStick_DPadEast = 210,
	K_EInputActionOrigin_Switch_RightStick_Move = 211,
	K_EInputActionOrigin_Switch_RightStick_Click = 212,
	K_EInputActionOrigin_Switch_RightStick_DPadNorth = 213,
	K_EInputActionOrigin_Switch_RightStick_DPadSouth = 214,
	K_EInputActionOrigin_Switch_RightStick_DPadWest = 215,
	K_EInputActionOrigin_Switch_RightStick_DPadEast = 216,
	K_EInputActionOrigin_Switch_DPad_North = 217,
	K_EInputActionOrigin_Switch_DPad_South = 218,
	K_EInputActionOrigin_Switch_DPad_West = 219,
	K_EInputActionOrigin_Switch_DPad_East = 220,
	K_EInputActionOrigin_Switch_ProGyro_Move = 221,
	K_EInputActionOrigin_Switch_ProGyro_Pitch = 222,
	K_EInputActionOrigin_Switch_ProGyro_Yaw = 223,
	K_EInputActionOrigin_Switch_ProGyro_Roll = 224,
	K_EInputActionOrigin_Switch_DPad_Move = 225,
	K_EInputActionOrigin_Switch_Reserved1 = 226,
	K_EInputActionOrigin_Switch_Reserved2 = 227,
	K_EInputActionOrigin_Switch_Reserved3 = 228,
	K_EInputActionOrigin_Switch_Reserved4 = 229,
	K_EInputActionOrigin_Switch_Reserved5 = 230,
	K_EInputActionOrigin_Switch_Reserved6 = 231,
	K_EInputActionOrigin_Switch_Reserved7 = 232,
	K_EInputActionOrigin_Switch_Reserved8 = 233,
	K_EInputActionOrigin_Switch_Reserved9 = 234,
	K_EInputActionOrigin_Switch_Reserved10 = 235,
	K_EInputActionOrigin_Switch_RightGyro_Move = 236,
	K_EInputActionOrigin_Switch_RightGyro_Pitch = 237,
	K_EInputActionOrigin_Switch_RightGyro_Yaw = 238,
	K_EInputActionOrigin_Switch_RightGyro_Roll = 239,
	K_EInputActionOrigin_Switch_LeftGyro_Move = 240,
	K_EInputActionOrigin_Switch_LeftGyro_Pitch = 241,
	K_EInputActionOrigin_Switch_LeftGyro_Yaw = 242,
	K_EInputActionOrigin_Switch_LeftGyro_Roll = 243,
	K_EInputActionOrigin_Switch_LeftGrip_Lower = 244,
	K_EInputActionOrigin_Switch_LeftGrip_Upper = 245,
	K_EInputActionOrigin_Switch_RightGrip_Lower = 246,
	K_EInputActionOrigin_Switch_RightGrip_Upper = 247,
	K_EInputActionOrigin_Switch_JoyConButton_N = 248,
	K_EInputActionOrigin_Switch_JoyConButton_E = 249,
	K_EInputActionOrigin_Switch_JoyConButton_S = 250,
	K_EInputActionOrigin_Switch_JoyConButton_W = 251,
	K_EInputActionOrigin_Switch_Reserved15 = 252,
	K_EInputActionOrigin_Switch_Reserved16 = 253,
	K_EInputActionOrigin_Switch_Reserved17 = 254,
	K_EInputActionOrigin_Switch_Reserved18 = 255,
	K_EInputActionOrigin_Switch_Reserved19 = 256,
	K_EInputActionOrigin_Switch_Reserved20 = 257,
	K_EInputActionOrigin_PS5_X = 258,
	K_EInputActionOrigin_PS5_Circle = 259,
	K_EInputActionOrigin_PS5_Triangle = 260,
	K_EInputActionOrigin_PS5_Square = 261,
	K_EInputActionOrigin_PS5_LeftBumper = 262,
	K_EInputActionOrigin_PS5_RightBumper = 263,
	K_EInputActionOrigin_PS5_Option = 264,
	K_EInputActionOrigin_PS5_Create = 265,
	K_EInputActionOrigin_PS5_Mute = 266,
	K_EInputActionOrigin_PS5_LeftPad_Touch = 267,
	K_EInputActionOrigin_PS5_LeftPad_Swipe = 268,
	K_EInputActionOrigin_PS5_LeftPad_Click = 269,
	K_EInputActionOrigin_PS5_LeftPad_DPadNorth = 270,
	K_EInputActionOrigin_PS5_LeftPad_DPadSouth = 271,
	K_EInputActionOrigin_PS5_LeftPad_DPadWest = 272,
	K_EInputActionOrigin_PS5_LeftPad_DPadEast = 273,
	K_EInputActionOrigin_PS5_RightPad_Touch = 274,
	K_EInputActionOrigin_PS5_RightPad_Swipe = 275,
	K_EInputActionOrigin_PS5_RightPad_Click = 276,
	K_EInputActionOrigin_PS5_RightPad_DPadNorth = 277,
	K_EInputActionOrigin_PS5_RightPad_DPadSouth = 278,
	K_EInputActionOrigin_PS5_RightPad_DPadWest = 279,
	K_EInputActionOrigin_PS5_RightPad_DPadEast = 280,
	K_EInputActionOrigin_PS5_CenterPad_Touch = 281,
	K_EInputActionOrigin_PS5_CenterPad_Swipe = 282,
	K_EInputActionOrigin_PS5_CenterPad_Click = 283,
	K_EInputActionOrigin_PS5_CenterPad_DPadNorth = 284,
	K_EInputActionOrigin_PS5_CenterPad_DPadSouth = 285,
	K_EInputActionOrigin_PS5_CenterPad_DPadWest = 286,
	K_EInputActionOrigin_PS5_CenterPad_DPadEast = 287,
	K_EInputActionOrigin_PS5_LeftTrigger_Pull = 288,
	K_EInputActionOrigin_PS5_LeftTrigger_Click = 289,
	K_EInputActionOrigin_PS5_RightTrigger_Pull = 290,
	K_EInputActionOrigin_PS5_RightTrigger_Click = 291,
	K_EInputActionOrigin_PS5_LeftStick_Move = 292,
	K_EInputActionOrigin_PS5_LeftStick_Click = 293,
	K_EInputActionOrigin_PS5_LeftStick_DPadNorth = 294,
	K_EInputActionOrigin_PS5_LeftStick_DPadSouth = 295,
	K_EInputActionOrigin_PS5_LeftStick_DPadWest = 296,
	K_EInputActionOrigin_PS5_LeftStick_DPadEast = 297,
	K_EInputActionOrigin_PS5_RightStick_Move = 298,
	K_EInputActionOrigin_PS5_RightStick_Click = 299,
	K_EInputActionOrigin_PS5_RightStick_DPadNorth = 300,
	K_EInputActionOrigin_PS5_RightStick_DPadSouth = 301,
	K_EInputActionOrigin_PS5_RightStick_DPadWest = 302,
	K_EInputActionOrigin_PS5_RightStick_DPadEast = 303,
	K_EInputActionOrigin_PS5_DPad_North = 304,
	K_EInputActionOrigin_PS5_DPad_South = 305,
	K_EInputActionOrigin_PS5_DPad_West = 306,
	K_EInputActionOrigin_PS5_DPad_East = 307,
	K_EInputActionOrigin_PS5_Gyro_Move = 308,
	K_EInputActionOrigin_PS5_Gyro_Pitch = 309,
	K_EInputActionOrigin_PS5_Gyro_Yaw = 310,
	K_EInputActionOrigin_PS5_Gyro_Roll = 311,
	K_EInputActionOrigin_PS5_DPad_Move = 312,
	K_EInputActionOrigin_PS5_LeftGrip = 313,
	K_EInputActionOrigin_PS5_RightGrip = 314,
	K_EInputActionOrigin_PS5_LeftFn = 315,
	K_EInputActionOrigin_PS5_RightFn = 316,
	K_EInputActionOrigin_PS5_Reserved5 = 317,
	K_EInputActionOrigin_PS5_Reserved6 = 318,
	K_EInputActionOrigin_PS5_Reserved7 = 319,
	K_EInputActionOrigin_PS5_Reserved8 = 320,
	K_EInputActionOrigin_PS5_Reserved9 = 321,
	K_EInputActionOrigin_PS5_Reserved10 = 322,
	K_EInputActionOrigin_PS5_Reserved11 = 323,
	K_EInputActionOrigin_PS5_Reserved12 = 324,
	K_EInputActionOrigin_PS5_Reserved13 = 325,
	K_EInputActionOrigin_PS5_Reserved14 = 326,
	K_EInputActionOrigin_PS5_Reserved15 = 327,
	K_EInputActionOrigin_PS5_Reserved16 = 328,
	K_EInputActionOrigin_PS5_Reserved17 = 329,
	K_EInputActionOrigin_PS5_Reserved18 = 330,
	K_EInputActionOrigin_PS5_Reserved19 = 331,
	K_EInputActionOrigin_PS5_Reserved20 = 332,
	K_EInputActionOrigin_SteamDeck_A = 333,
	K_EInputActionOrigin_SteamDeck_B = 334,
	K_EInputActionOrigin_SteamDeck_X = 335,
	K_EInputActionOrigin_SteamDeck_Y = 336,
	K_EInputActionOrigin_SteamDeck_L1 = 337,
	K_EInputActionOrigin_SteamDeck_R1 = 338,
	K_EInputActionOrigin_SteamDeck_Menu = 339,
	K_EInputActionOrigin_SteamDeck_View = 340,
	K_EInputActionOrigin_SteamDeck_LeftPad_Touch = 341,
	K_EInputActionOrigin_SteamDeck_LeftPad_Swipe = 342,
	K_EInputActionOrigin_SteamDeck_LeftPad_Click = 343,
	K_EInputActionOrigin_SteamDeck_LeftPad_DPadNorth = 344,
	K_EInputActionOrigin_SteamDeck_LeftPad_DPadSouth = 345,
	K_EInputActionOrigin_SteamDeck_LeftPad_DPadWest = 346,
	K_EInputActionOrigin_SteamDeck_LeftPad_DPadEast = 347,
	K_EInputActionOrigin_SteamDeck_RightPad_Touch = 348,
	K_EInputActionOrigin_SteamDeck_RightPad_Swipe = 349,
	K_EInputActionOrigin_SteamDeck_RightPad_Click = 350,
	K_EInputActionOrigin_SteamDeck_RightPad_DPadNorth = 351,
	K_EInputActionOrigin_SteamDeck_RightPad_DPadSouth = 352,
	K_EInputActionOrigin_SteamDeck_RightPad_DPadWest = 353,
	K_EInputActionOrigin_SteamDeck_RightPad_DPadEast = 354,
	K_EInputActionOrigin_SteamDeck_L2_SoftPull = 355,
	K_EInputActionOrigin_SteamDeck_L2 = 356,
	K_EInputActionOrigin_SteamDeck_R2_SoftPull = 357,
	K_EInputActionOrigin_SteamDeck_R2 = 358,
	K_EInputActionOrigin_SteamDeck_LeftStick_Move = 359,
	K_EInputActionOrigin_SteamDeck_L3 = 360,
	K_EInputActionOrigin_SteamDeck_LeftStick_DPadNorth = 361,
	K_EInputActionOrigin_SteamDeck_LeftStick_DPadSouth = 362,
	K_EInputActionOrigin_SteamDeck_LeftStick_DPadWest = 363,
	K_EInputActionOrigin_SteamDeck_LeftStick_DPadEast = 364,
	K_EInputActionOrigin_SteamDeck_LeftStick_Touch = 365,
	K_EInputActionOrigin_SteamDeck_RightStick_Move = 366,
	K_EInputActionOrigin_SteamDeck_R3 = 367,
	K_EInputActionOrigin_SteamDeck_RightStick_DPadNorth = 368,
	K_EInputActionOrigin_SteamDeck_RightStick_DPadSouth = 369,
	K_EInputActionOrigin_SteamDeck_RightStick_DPadWest = 370,
	K_EInputActionOrigin_SteamDeck_RightStick_DPadEast = 371,
	K_EInputActionOrigin_SteamDeck_RightStick_Touch = 372,
	K_EInputActionOrigin_SteamDeck_L4 = 373,
	K_EInputActionOrigin_SteamDeck_R4 = 374,
	K_EInputActionOrigin_SteamDeck_L5 = 375,
	K_EInputActionOrigin_SteamDeck_R5 = 376,
	K_EInputActionOrigin_SteamDeck_DPad_Move = 377,
	K_EInputActionOrigin_SteamDeck_DPad_North = 378,
	K_EInputActionOrigin_SteamDeck_DPad_South = 379,
	K_EInputActionOrigin_SteamDeck_DPad_West = 380,
	K_EInputActionOrigin_SteamDeck_DPad_East = 381,
	K_EInputActionOrigin_SteamDeck_Gyro_Move = 382,
	K_EInputActionOrigin_SteamDeck_Gyro_Pitch = 383,
	K_EInputActionOrigin_SteamDeck_Gyro_Yaw = 384,
	K_EInputActionOrigin_SteamDeck_Gyro_Roll = 385,
	K_EInputActionOrigin_SteamDeck_Reserved1 = 386,
	K_EInputActionOrigin_SteamDeck_Reserved2 = 387,
	K_EInputActionOrigin_SteamDeck_Reserved3 = 388,
	K_EInputActionOrigin_SteamDeck_Reserved4 = 389,
	K_EInputActionOrigin_SteamDeck_Reserved5 = 390,
	K_EInputActionOrigin_SteamDeck_Reserved6 = 391,
	K_EInputActionOrigin_SteamDeck_Reserved7 = 392,
	K_EInputActionOrigin_SteamDeck_Reserved8 = 393,
	K_EInputActionOrigin_SteamDeck_Reserved9 = 394,
	K_EInputActionOrigin_SteamDeck_Reserved10 = 395,
	K_EInputActionOrigin_SteamDeck_Reserved11 = 396,
	K_EInputActionOrigin_SteamDeck_Reserved12 = 397,
	K_EInputActionOrigin_SteamDeck_Reserved13 = 398,
	K_EInputActionOrigin_SteamDeck_Reserved14 = 399,
	K_EInputActionOrigin_SteamDeck_Reserved15 = 400,
	K_EInputActionOrigin_SteamDeck_Reserved16 = 401,
	K_EInputActionOrigin_SteamDeck_Reserved17 = 402,
	K_EInputActionOrigin_SteamDeck_Reserved18 = 403,
	K_EInputActionOrigin_SteamDeck_Reserved19 = 404,
	K_EInputActionOrigin_SteamDeck_Reserved20 = 405,
	K_EInputActionOrigin_Count = 406,
	K_EInputActionOrigin_MaximumPossibleValue = 32767,
 };
Q_ENUM(EInputActionOrigin)

enum class EXboxOrigin
{	
	K_EXboxOrigin_A = 0,
	K_EXboxOrigin_B = 1,
	K_EXboxOrigin_X = 2,
	K_EXboxOrigin_Y = 3,
	K_EXboxOrigin_LeftBumper = 4,
	K_EXboxOrigin_RightBumper = 5,
	K_EXboxOrigin_Menu = 6,
	K_EXboxOrigin_View = 7,
	K_EXboxOrigin_LeftTrigger_Pull = 8,
	K_EXboxOrigin_LeftTrigger_Click = 9,
	K_EXboxOrigin_RightTrigger_Pull = 10,
	K_EXboxOrigin_RightTrigger_Click = 11,
	K_EXboxOrigin_LeftStick_Move = 12,
	K_EXboxOrigin_LeftStick_Click = 13,
	K_EXboxOrigin_LeftStick_DPadNorth = 14,
	K_EXboxOrigin_LeftStick_DPadSouth = 15,
	K_EXboxOrigin_LeftStick_DPadWest = 16,
	K_EXboxOrigin_LeftStick_DPadEast = 17,
	K_EXboxOrigin_RightStick_Move = 18,
	K_EXboxOrigin_RightStick_Click = 19,
	K_EXboxOrigin_RightStick_DPadNorth = 20,
	K_EXboxOrigin_RightStick_DPadSouth = 21,
	K_EXboxOrigin_RightStick_DPadWest = 22,
	K_EXboxOrigin_RightStick_DPadEast = 23,
	K_EXboxOrigin_DPad_North = 24,
	K_EXboxOrigin_DPad_South = 25,
	K_EXboxOrigin_DPad_West = 26,
	K_EXboxOrigin_DPad_East = 27,
	K_EXboxOrigin_Count = 28,
 };
Q_ENUM(EXboxOrigin)

enum class ESteamControllerPad
{	
	K_ESteamControllerPad_Left = 0,
	K_ESteamControllerPad_Right = 1,
 };
Q_ENUM(ESteamControllerPad)

enum class EControllerHapticLocation
{	
	K_EControllerHapticLocation_Left = 1,
	K_EControllerHapticLocation_Right = 2,
	K_EControllerHapticLocation_Both = 3,
 };
Q_ENUM(EControllerHapticLocation)

enum class EControllerHapticType
{	
	K_EControllerHapticType_Off = 0,
	K_EControllerHapticType_Tick = 1,
	K_EControllerHapticType_Click = 2,
 };
Q_ENUM(EControllerHapticType)

enum class ESteamInputType
{	
	K_ESteamInputType_Unknown = 0,
	K_ESteamInputType_SteamController = 1,
	K_ESteamInputType_XBox360Controller = 2,
	K_ESteamInputType_XBoxOneController = 3,
	K_ESteamInputType_GenericGamepad = 4,
	K_ESteamInputType_PS4Controller = 5,
	K_ESteamInputType_AppleMFiController = 6,
	K_ESteamInputType_AndroidController = 7,
	K_ESteamInputType_SwitchJoyConPair = 8,
	K_ESteamInputType_SwitchJoyConSingle = 9,
	K_ESteamInputType_SwitchProController = 10,
	K_ESteamInputType_MobileTouch = 11,
	K_ESteamInputType_PS3Controller = 12,
	K_ESteamInputType_PS5Controller = 13,
	K_ESteamInputType_SteamDeckController = 14,
	K_ESteamInputType_Count = 15,
	K_ESteamInputType_MaximumPossibleValue = 255,
 };
Q_ENUM(ESteamInputType)

enum class ESteamInputConfigurationEnableType
{	
	K_ESteamInputConfigurationEnableType_None = 0,
	K_ESteamInputConfigurationEnableType_Playstation = 1,
	K_ESteamInputConfigurationEnableType_Xbox = 2,
	K_ESteamInputConfigurationEnableType_Generic = 4,
	K_ESteamInputConfigurationEnableType_Switch = 8,
 };
Q_ENUM(ESteamInputConfigurationEnableType)

enum class ESteamInputLEDFlag
{	
	K_ESteamInputLEDFlag_SetColor = 0,
	K_ESteamInputLEDFlag_RestoreUserDefault = 1,
 };
Q_ENUM(ESteamInputLEDFlag)

enum class ESteamInputGlyphSize
{	
	K_ESteamInputGlyphSize_Small = 0,
	K_ESteamInputGlyphSize_Medium = 1,
	K_ESteamInputGlyphSize_Large = 2,
	K_ESteamInputGlyphSize_Count = 3,
 };
Q_ENUM(ESteamInputGlyphSize)

enum class ESteamInputGlyphStyle
{	
	ESteamInputGlyphStyle_Knockout = 0,
	ESteamInputGlyphStyle_Light = 1,
	ESteamInputGlyphStyle_Dark = 2,
	ESteamInputGlyphStyle_NeutralColorABXY = 16,
	ESteamInputGlyphStyle_SolidABXY = 32,
 };
Q_ENUM(ESteamInputGlyphStyle)

enum class ESteamInputActionEventType
{	
	ESteamInputActionEventType_DigitalAction = 0,
	ESteamInputActionEventType_AnalogAction = 1,
 };
Q_ENUM(ESteamInputActionEventType)

enum class EControllerActionOrigin
{	
	K_EControllerActionOrigin_None = 0,
	K_EControllerActionOrigin_A = 1,
	K_EControllerActionOrigin_B = 2,
	K_EControllerActionOrigin_X = 3,
	K_EControllerActionOrigin_Y = 4,
	K_EControllerActionOrigin_LeftBumper = 5,
	K_EControllerActionOrigin_RightBumper = 6,
	K_EControllerActionOrigin_LeftGrip = 7,
	K_EControllerActionOrigin_RightGrip = 8,
	K_EControllerActionOrigin_Start = 9,
	K_EControllerActionOrigin_Back = 10,
	K_EControllerActionOrigin_LeftPad_Touch = 11,
	K_EControllerActionOrigin_LeftPad_Swipe = 12,
	K_EControllerActionOrigin_LeftPad_Click = 13,
	K_EControllerActionOrigin_LeftPad_DPadNorth = 14,
	K_EControllerActionOrigin_LeftPad_DPadSouth = 15,
	K_EControllerActionOrigin_LeftPad_DPadWest = 16,
	K_EControllerActionOrigin_LeftPad_DPadEast = 17,
	K_EControllerActionOrigin_RightPad_Touch = 18,
	K_EControllerActionOrigin_RightPad_Swipe = 19,
	K_EControllerActionOrigin_RightPad_Click = 20,
	K_EControllerActionOrigin_RightPad_DPadNorth = 21,
	K_EControllerActionOrigin_RightPad_DPadSouth = 22,
	K_EControllerActionOrigin_RightPad_DPadWest = 23,
	K_EControllerActionOrigin_RightPad_DPadEast = 24,
	K_EControllerActionOrigin_LeftTrigger_Pull = 25,
	K_EControllerActionOrigin_LeftTrigger_Click = 26,
	K_EControllerActionOrigin_RightTrigger_Pull = 27,
	K_EControllerActionOrigin_RightTrigger_Click = 28,
	K_EControllerActionOrigin_LeftStick_Move = 29,
	K_EControllerActionOrigin_LeftStick_Click = 30,
	K_EControllerActionOrigin_LeftStick_DPadNorth = 31,
	K_EControllerActionOrigin_LeftStick_DPadSouth = 32,
	K_EControllerActionOrigin_LeftStick_DPadWest = 33,
	K_EControllerActionOrigin_LeftStick_DPadEast = 34,
	K_EControllerActionOrigin_Gyro_Move = 35,
	K_EControllerActionOrigin_Gyro_Pitch = 36,
	K_EControllerActionOrigin_Gyro_Yaw = 37,
	K_EControllerActionOrigin_Gyro_Roll = 38,
	K_EControllerActionOrigin_PS4_X = 39,
	K_EControllerActionOrigin_PS4_Circle = 40,
	K_EControllerActionOrigin_PS4_Triangle = 41,
	K_EControllerActionOrigin_PS4_Square = 42,
	K_EControllerActionOrigin_PS4_LeftBumper = 43,
	K_EControllerActionOrigin_PS4_RightBumper = 44,
	K_EControllerActionOrigin_PS4_Options = 45,
	K_EControllerActionOrigin_PS4_Share = 46,
	K_EControllerActionOrigin_PS4_LeftPad_Touch = 47,
	K_EControllerActionOrigin_PS4_LeftPad_Swipe = 48,
	K_EControllerActionOrigin_PS4_LeftPad_Click = 49,
	K_EControllerActionOrigin_PS4_LeftPad_DPadNorth = 50,
	K_EControllerActionOrigin_PS4_LeftPad_DPadSouth = 51,
	K_EControllerActionOrigin_PS4_LeftPad_DPadWest = 52,
	K_EControllerActionOrigin_PS4_LeftPad_DPadEast = 53,
	K_EControllerActionOrigin_PS4_RightPad_Touch = 54,
	K_EControllerActionOrigin_PS4_RightPad_Swipe = 55,
	K_EControllerActionOrigin_PS4_RightPad_Click = 56,
	K_EControllerActionOrigin_PS4_RightPad_DPadNorth = 57,
	K_EControllerActionOrigin_PS4_RightPad_DPadSouth = 58,
	K_EControllerActionOrigin_PS4_RightPad_DPadWest = 59,
	K_EControllerActionOrigin_PS4_RightPad_DPadEast = 60,
	K_EControllerActionOrigin_PS4_CenterPad_Touch = 61,
	K_EControllerActionOrigin_PS4_CenterPad_Swipe = 62,
	K_EControllerActionOrigin_PS4_CenterPad_Click = 63,
	K_EControllerActionOrigin_PS4_CenterPad_DPadNorth = 64,
	K_EControllerActionOrigin_PS4_CenterPad_DPadSouth = 65,
	K_EControllerActionOrigin_PS4_CenterPad_DPadWest = 66,
	K_EControllerActionOrigin_PS4_CenterPad_DPadEast = 67,
	K_EControllerActionOrigin_PS4_LeftTrigger_Pull = 68,
	K_EControllerActionOrigin_PS4_LeftTrigger_Click = 69,
	K_EControllerActionOrigin_PS4_RightTrigger_Pull = 70,
	K_EControllerActionOrigin_PS4_RightTrigger_Click = 71,
	K_EControllerActionOrigin_PS4_LeftStick_Move = 72,
	K_EControllerActionOrigin_PS4_LeftStick_Click = 73,
	K_EControllerActionOrigin_PS4_LeftStick_DPadNorth = 74,
	K_EControllerActionOrigin_PS4_LeftStick_DPadSouth = 75,
	K_EControllerActionOrigin_PS4_LeftStick_DPadWest = 76,
	K_EControllerActionOrigin_PS4_LeftStick_DPadEast = 77,
	K_EControllerActionOrigin_PS4_RightStick_Move = 78,
	K_EControllerActionOrigin_PS4_RightStick_Click = 79,
	K_EControllerActionOrigin_PS4_RightStick_DPadNorth = 80,
	K_EControllerActionOrigin_PS4_RightStick_DPadSouth = 81,
	K_EControllerActionOrigin_PS4_RightStick_DPadWest = 82,
	K_EControllerActionOrigin_PS4_RightStick_DPadEast = 83,
	K_EControllerActionOrigin_PS4_DPad_North = 84,
	K_EControllerActionOrigin_PS4_DPad_South = 85,
	K_EControllerActionOrigin_PS4_DPad_West = 86,
	K_EControllerActionOrigin_PS4_DPad_East = 87,
	K_EControllerActionOrigin_PS4_Gyro_Move = 88,
	K_EControllerActionOrigin_PS4_Gyro_Pitch = 89,
	K_EControllerActionOrigin_PS4_Gyro_Yaw = 90,
	K_EControllerActionOrigin_PS4_Gyro_Roll = 91,
	K_EControllerActionOrigin_XBoxOne_A = 92,
	K_EControllerActionOrigin_XBoxOne_B = 93,
	K_EControllerActionOrigin_XBoxOne_X = 94,
	K_EControllerActionOrigin_XBoxOne_Y = 95,
	K_EControllerActionOrigin_XBoxOne_LeftBumper = 96,
	K_EControllerActionOrigin_XBoxOne_RightBumper = 97,
	K_EControllerActionOrigin_XBoxOne_Menu = 98,
	K_EControllerActionOrigin_XBoxOne_View = 99,
	K_EControllerActionOrigin_XBoxOne_LeftTrigger_Pull = 100,
	K_EControllerActionOrigin_XBoxOne_LeftTrigger_Click = 101,
	K_EControllerActionOrigin_XBoxOne_RightTrigger_Pull = 102,
	K_EControllerActionOrigin_XBoxOne_RightTrigger_Click = 103,
	K_EControllerActionOrigin_XBoxOne_LeftStick_Move = 104,
	K_EControllerActionOrigin_XBoxOne_LeftStick_Click = 105,
	K_EControllerActionOrigin_XBoxOne_LeftStick_DPadNorth = 106,
	K_EControllerActionOrigin_XBoxOne_LeftStick_DPadSouth = 107,
	K_EControllerActionOrigin_XBoxOne_LeftStick_DPadWest = 108,
	K_EControllerActionOrigin_XBoxOne_LeftStick_DPadEast = 109,
	K_EControllerActionOrigin_XBoxOne_RightStick_Move = 110,
	K_EControllerActionOrigin_XBoxOne_RightStick_Click = 111,
	K_EControllerActionOrigin_XBoxOne_RightStick_DPadNorth = 112,
	K_EControllerActionOrigin_XBoxOne_RightStick_DPadSouth = 113,
	K_EControllerActionOrigin_XBoxOne_RightStick_DPadWest = 114,
	K_EControllerActionOrigin_XBoxOne_RightStick_DPadEast = 115,
	K_EControllerActionOrigin_XBoxOne_DPad_North = 116,
	K_EControllerActionOrigin_XBoxOne_DPad_South = 117,
	K_EControllerActionOrigin_XBoxOne_DPad_West = 118,
	K_EControllerActionOrigin_XBoxOne_DPad_East = 119,
	K_EControllerActionOrigin_XBox360_A = 120,
	K_EControllerActionOrigin_XBox360_B = 121,
	K_EControllerActionOrigin_XBox360_X = 122,
	K_EControllerActionOrigin_XBox360_Y = 123,
	K_EControllerActionOrigin_XBox360_LeftBumper = 124,
	K_EControllerActionOrigin_XBox360_RightBumper = 125,
	K_EControllerActionOrigin_XBox360_Start = 126,
	K_EControllerActionOrigin_XBox360_Back = 127,
	K_EControllerActionOrigin_XBox360_LeftTrigger_Pull = 128,
	K_EControllerActionOrigin_XBox360_LeftTrigger_Click = 129,
	K_EControllerActionOrigin_XBox360_RightTrigger_Pull = 130,
	K_EControllerActionOrigin_XBox360_RightTrigger_Click = 131,
	K_EControllerActionOrigin_XBox360_LeftStick_Move = 132,
	K_EControllerActionOrigin_XBox360_LeftStick_Click = 133,
	K_EControllerActionOrigin_XBox360_LeftStick_DPadNorth = 134,
	K_EControllerActionOrigin_XBox360_LeftStick_DPadSouth = 135,
	K_EControllerActionOrigin_XBox360_LeftStick_DPadWest = 136,
	K_EControllerActionOrigin_XBox360_LeftStick_DPadEast = 137,
	K_EControllerActionOrigin_XBox360_RightStick_Move = 138,
	K_EControllerActionOrigin_XBox360_RightStick_Click = 139,
	K_EControllerActionOrigin_XBox360_RightStick_DPadNorth = 140,
	K_EControllerActionOrigin_XBox360_RightStick_DPadSouth = 141,
	K_EControllerActionOrigin_XBox360_RightStick_DPadWest = 142,
	K_EControllerActionOrigin_XBox360_RightStick_DPadEast = 143,
	K_EControllerActionOrigin_XBox360_DPad_North = 144,
	K_EControllerActionOrigin_XBox360_DPad_South = 145,
	K_EControllerActionOrigin_XBox360_DPad_West = 146,
	K_EControllerActionOrigin_XBox360_DPad_East = 147,
	K_EControllerActionOrigin_SteamV2_A = 148,
	K_EControllerActionOrigin_SteamV2_B = 149,
	K_EControllerActionOrigin_SteamV2_X = 150,
	K_EControllerActionOrigin_SteamV2_Y = 151,
	K_EControllerActionOrigin_SteamV2_LeftBumper = 152,
	K_EControllerActionOrigin_SteamV2_RightBumper = 153,
	K_EControllerActionOrigin_SteamV2_LeftGrip_Lower = 154,
	K_EControllerActionOrigin_SteamV2_LeftGrip_Upper = 155,
	K_EControllerActionOrigin_SteamV2_RightGrip_Lower = 156,
	K_EControllerActionOrigin_SteamV2_RightGrip_Upper = 157,
	K_EControllerActionOrigin_SteamV2_LeftBumper_Pressure = 158,
	K_EControllerActionOrigin_SteamV2_RightBumper_Pressure = 159,
	K_EControllerActionOrigin_SteamV2_LeftGrip_Pressure = 160,
	K_EControllerActionOrigin_SteamV2_RightGrip_Pressure = 161,
	K_EControllerActionOrigin_SteamV2_LeftGrip_Upper_Pressure = 162,
	K_EControllerActionOrigin_SteamV2_RightGrip_Upper_Pressure = 163,
	K_EControllerActionOrigin_SteamV2_Start = 164,
	K_EControllerActionOrigin_SteamV2_Back = 165,
	K_EControllerActionOrigin_SteamV2_LeftPad_Touch = 166,
	K_EControllerActionOrigin_SteamV2_LeftPad_Swipe = 167,
	K_EControllerActionOrigin_SteamV2_LeftPad_Click = 168,
	K_EControllerActionOrigin_SteamV2_LeftPad_Pressure = 169,
	K_EControllerActionOrigin_SteamV2_LeftPad_DPadNorth = 170,
	K_EControllerActionOrigin_SteamV2_LeftPad_DPadSouth = 171,
	K_EControllerActionOrigin_SteamV2_LeftPad_DPadWest = 172,
	K_EControllerActionOrigin_SteamV2_LeftPad_DPadEast = 173,
	K_EControllerActionOrigin_SteamV2_RightPad_Touch = 174,
	K_EControllerActionOrigin_SteamV2_RightPad_Swipe = 175,
	K_EControllerActionOrigin_SteamV2_RightPad_Click = 176,
	K_EControllerActionOrigin_SteamV2_RightPad_Pressure = 177,
	K_EControllerActionOrigin_SteamV2_RightPad_DPadNorth = 178,
	K_EControllerActionOrigin_SteamV2_RightPad_DPadSouth = 179,
	K_EControllerActionOrigin_SteamV2_RightPad_DPadWest = 180,
	K_EControllerActionOrigin_SteamV2_RightPad_DPadEast = 181,
	K_EControllerActionOrigin_SteamV2_LeftTrigger_Pull = 182,
	K_EControllerActionOrigin_SteamV2_LeftTrigger_Click = 183,
	K_EControllerActionOrigin_SteamV2_RightTrigger_Pull = 184,
	K_EControllerActionOrigin_SteamV2_RightTrigger_Click = 185,
	K_EControllerActionOrigin_SteamV2_LeftStick_Move = 186,
	K_EControllerActionOrigin_SteamV2_LeftStick_Click = 187,
	K_EControllerActionOrigin_SteamV2_LeftStick_DPadNorth = 188,
	K_EControllerActionOrigin_SteamV2_LeftStick_DPadSouth = 189,
	K_EControllerActionOrigin_SteamV2_LeftStick_DPadWest = 190,
	K_EControllerActionOrigin_SteamV2_LeftStick_DPadEast = 191,
	K_EControllerActionOrigin_SteamV2_Gyro_Move = 192,
	K_EControllerActionOrigin_SteamV2_Gyro_Pitch = 193,
	K_EControllerActionOrigin_SteamV2_Gyro_Yaw = 194,
	K_EControllerActionOrigin_SteamV2_Gyro_Roll = 195,
	K_EControllerActionOrigin_Switch_A = 196,
	K_EControllerActionOrigin_Switch_B = 197,
	K_EControllerActionOrigin_Switch_X = 198,
	K_EControllerActionOrigin_Switch_Y = 199,
	K_EControllerActionOrigin_Switch_LeftBumper = 200,
	K_EControllerActionOrigin_Switch_RightBumper = 201,
	K_EControllerActionOrigin_Switch_Plus = 202,
	K_EControllerActionOrigin_Switch_Minus = 203,
	K_EControllerActionOrigin_Switch_Capture = 204,
	K_EControllerActionOrigin_Switch_LeftTrigger_Pull = 205,
	K_EControllerActionOrigin_Switch_LeftTrigger_Click = 206,
	K_EControllerActionOrigin_Switch_RightTrigger_Pull = 207,
	K_EControllerActionOrigin_Switch_RightTrigger_Click = 208,
	K_EControllerActionOrigin_Switch_LeftStick_Move = 209,
	K_EControllerActionOrigin_Switch_LeftStick_Click = 210,
	K_EControllerActionOrigin_Switch_LeftStick_DPadNorth = 211,
	K_EControllerActionOrigin_Switch_LeftStick_DPadSouth = 212,
	K_EControllerActionOrigin_Switch_LeftStick_DPadWest = 213,
	K_EControllerActionOrigin_Switch_LeftStick_DPadEast = 214,
	K_EControllerActionOrigin_Switch_RightStick_Move = 215,
	K_EControllerActionOrigin_Switch_RightStick_Click = 216,
	K_EControllerActionOrigin_Switch_RightStick_DPadNorth = 217,
	K_EControllerActionOrigin_Switch_RightStick_DPadSouth = 218,
	K_EControllerActionOrigin_Switch_RightStick_DPadWest = 219,
	K_EControllerActionOrigin_Switch_RightStick_DPadEast = 220,
	K_EControllerActionOrigin_Switch_DPad_North = 221,
	K_EControllerActionOrigin_Switch_DPad_South = 222,
	K_EControllerActionOrigin_Switch_DPad_West = 223,
	K_EControllerActionOrigin_Switch_DPad_East = 224,
	K_EControllerActionOrigin_Switch_ProGyro_Move = 225,
	K_EControllerActionOrigin_Switch_ProGyro_Pitch = 226,
	K_EControllerActionOrigin_Switch_ProGyro_Yaw = 227,
	K_EControllerActionOrigin_Switch_ProGyro_Roll = 228,
	K_EControllerActionOrigin_Switch_RightGyro_Move = 229,
	K_EControllerActionOrigin_Switch_RightGyro_Pitch = 230,
	K_EControllerActionOrigin_Switch_RightGyro_Yaw = 231,
	K_EControllerActionOrigin_Switch_RightGyro_Roll = 232,
	K_EControllerActionOrigin_Switch_LeftGyro_Move = 233,
	K_EControllerActionOrigin_Switch_LeftGyro_Pitch = 234,
	K_EControllerActionOrigin_Switch_LeftGyro_Yaw = 235,
	K_EControllerActionOrigin_Switch_LeftGyro_Roll = 236,
	K_EControllerActionOrigin_Switch_LeftGrip_Lower = 237,
	K_EControllerActionOrigin_Switch_LeftGrip_Upper = 238,
	K_EControllerActionOrigin_Switch_RightGrip_Lower = 239,
	K_EControllerActionOrigin_Switch_RightGrip_Upper = 240,
	K_EControllerActionOrigin_PS4_DPad_Move = 241,
	K_EControllerActionOrigin_XBoxOne_DPad_Move = 242,
	K_EControllerActionOrigin_XBox360_DPad_Move = 243,
	K_EControllerActionOrigin_Switch_DPad_Move = 244,
	K_EControllerActionOrigin_PS5_X = 245,
	K_EControllerActionOrigin_PS5_Circle = 246,
	K_EControllerActionOrigin_PS5_Triangle = 247,
	K_EControllerActionOrigin_PS5_Square = 248,
	K_EControllerActionOrigin_PS5_LeftBumper = 249,
	K_EControllerActionOrigin_PS5_RightBumper = 250,
	K_EControllerActionOrigin_PS5_Option = 251,
	K_EControllerActionOrigin_PS5_Create = 252,
	K_EControllerActionOrigin_PS5_Mute = 253,
	K_EControllerActionOrigin_PS5_LeftPad_Touch = 254,
	K_EControllerActionOrigin_PS5_LeftPad_Swipe = 255,
	K_EControllerActionOrigin_PS5_LeftPad_Click = 256,
	K_EControllerActionOrigin_PS5_LeftPad_DPadNorth = 257,
	K_EControllerActionOrigin_PS5_LeftPad_DPadSouth = 258,
	K_EControllerActionOrigin_PS5_LeftPad_DPadWest = 259,
	K_EControllerActionOrigin_PS5_LeftPad_DPadEast = 260,
	K_EControllerActionOrigin_PS5_RightPad_Touch = 261,
	K_EControllerActionOrigin_PS5_RightPad_Swipe = 262,
	K_EControllerActionOrigin_PS5_RightPad_Click = 263,
	K_EControllerActionOrigin_PS5_RightPad_DPadNorth = 264,
	K_EControllerActionOrigin_PS5_RightPad_DPadSouth = 265,
	K_EControllerActionOrigin_PS5_RightPad_DPadWest = 266,
	K_EControllerActionOrigin_PS5_RightPad_DPadEast = 267,
	K_EControllerActionOrigin_PS5_CenterPad_Touch = 268,
	K_EControllerActionOrigin_PS5_CenterPad_Swipe = 269,
	K_EControllerActionOrigin_PS5_CenterPad_Click = 270,
	K_EControllerActionOrigin_PS5_CenterPad_DPadNorth = 271,
	K_EControllerActionOrigin_PS5_CenterPad_DPadSouth = 272,
	K_EControllerActionOrigin_PS5_CenterPad_DPadWest = 273,
	K_EControllerActionOrigin_PS5_CenterPad_DPadEast = 274,
	K_EControllerActionOrigin_PS5_LeftTrigger_Pull = 275,
	K_EControllerActionOrigin_PS5_LeftTrigger_Click = 276,
	K_EControllerActionOrigin_PS5_RightTrigger_Pull = 277,
	K_EControllerActionOrigin_PS5_RightTrigger_Click = 278,
	K_EControllerActionOrigin_PS5_LeftStick_Move = 279,
	K_EControllerActionOrigin_PS5_LeftStick_Click = 280,
	K_EControllerActionOrigin_PS5_LeftStick_DPadNorth = 281,
	K_EControllerActionOrigin_PS5_LeftStick_DPadSouth = 282,
	K_EControllerActionOrigin_PS5_LeftStick_DPadWest = 283,
	K_EControllerActionOrigin_PS5_LeftStick_DPadEast = 284,
	K_EControllerActionOrigin_PS5_RightStick_Move = 285,
	K_EControllerActionOrigin_PS5_RightStick_Click = 286,
	K_EControllerActionOrigin_PS5_RightStick_DPadNorth = 287,
	K_EControllerActionOrigin_PS5_RightStick_DPadSouth = 288,
	K_EControllerActionOrigin_PS5_RightStick_DPadWest = 289,
	K_EControllerActionOrigin_PS5_RightStick_DPadEast = 290,
	K_EControllerActionOrigin_PS5_DPad_Move = 291,
	K_EControllerActionOrigin_PS5_DPad_North = 292,
	K_EControllerActionOrigin_PS5_DPad_South = 293,
	K_EControllerActionOrigin_PS5_DPad_West = 294,
	K_EControllerActionOrigin_PS5_DPad_East = 295,
	K_EControllerActionOrigin_PS5_Gyro_Move = 296,
	K_EControllerActionOrigin_PS5_Gyro_Pitch = 297,
	K_EControllerActionOrigin_PS5_Gyro_Yaw = 298,
	K_EControllerActionOrigin_PS5_Gyro_Roll = 299,
	K_EControllerActionOrigin_XBoxOne_LeftGrip_Lower = 300,
	K_EControllerActionOrigin_XBoxOne_LeftGrip_Upper = 301,
	K_EControllerActionOrigin_XBoxOne_RightGrip_Lower = 302,
	K_EControllerActionOrigin_XBoxOne_RightGrip_Upper = 303,
	K_EControllerActionOrigin_XBoxOne_Share = 304,
	K_EControllerActionOrigin_SteamDeck_A = 305,
	K_EControllerActionOrigin_SteamDeck_B = 306,
	K_EControllerActionOrigin_SteamDeck_X = 307,
	K_EControllerActionOrigin_SteamDeck_Y = 308,
	K_EControllerActionOrigin_SteamDeck_L1 = 309,
	K_EControllerActionOrigin_SteamDeck_R1 = 310,
	K_EControllerActionOrigin_SteamDeck_Menu = 311,
	K_EControllerActionOrigin_SteamDeck_View = 312,
	K_EControllerActionOrigin_SteamDeck_LeftPad_Touch = 313,
	K_EControllerActionOrigin_SteamDeck_LeftPad_Swipe = 314,
	K_EControllerActionOrigin_SteamDeck_LeftPad_Click = 315,
	K_EControllerActionOrigin_SteamDeck_LeftPad_DPadNorth = 316,
	K_EControllerActionOrigin_SteamDeck_LeftPad_DPadSouth = 317,
	K_EControllerActionOrigin_SteamDeck_LeftPad_DPadWest = 318,
	K_EControllerActionOrigin_SteamDeck_LeftPad_DPadEast = 319,
	K_EControllerActionOrigin_SteamDeck_RightPad_Touch = 320,
	K_EControllerActionOrigin_SteamDeck_RightPad_Swipe = 321,
	K_EControllerActionOrigin_SteamDeck_RightPad_Click = 322,
	K_EControllerActionOrigin_SteamDeck_RightPad_DPadNorth = 323,
	K_EControllerActionOrigin_SteamDeck_RightPad_DPadSouth = 324,
	K_EControllerActionOrigin_SteamDeck_RightPad_DPadWest = 325,
	K_EControllerActionOrigin_SteamDeck_RightPad_DPadEast = 326,
	K_EControllerActionOrigin_SteamDeck_L2_SoftPull = 327,
	K_EControllerActionOrigin_SteamDeck_L2 = 328,
	K_EControllerActionOrigin_SteamDeck_R2_SoftPull = 329,
	K_EControllerActionOrigin_SteamDeck_R2 = 330,
	K_EControllerActionOrigin_SteamDeck_LeftStick_Move = 331,
	K_EControllerActionOrigin_SteamDeck_L3 = 332,
	K_EControllerActionOrigin_SteamDeck_LeftStick_DPadNorth = 333,
	K_EControllerActionOrigin_SteamDeck_LeftStick_DPadSouth = 334,
	K_EControllerActionOrigin_SteamDeck_LeftStick_DPadWest = 335,
	K_EControllerActionOrigin_SteamDeck_LeftStick_DPadEast = 336,
	K_EControllerActionOrigin_SteamDeck_LeftStick_Touch = 337,
	K_EControllerActionOrigin_SteamDeck_RightStick_Move = 338,
	K_EControllerActionOrigin_SteamDeck_R3 = 339,
	K_EControllerActionOrigin_SteamDeck_RightStick_DPadNorth = 340,
	K_EControllerActionOrigin_SteamDeck_RightStick_DPadSouth = 341,
	K_EControllerActionOrigin_SteamDeck_RightStick_DPadWest = 342,
	K_EControllerActionOrigin_SteamDeck_RightStick_DPadEast = 343,
	K_EControllerActionOrigin_SteamDeck_RightStick_Touch = 344,
	K_EControllerActionOrigin_SteamDeck_L4 = 345,
	K_EControllerActionOrigin_SteamDeck_R4 = 346,
	K_EControllerActionOrigin_SteamDeck_L5 = 347,
	K_EControllerActionOrigin_SteamDeck_R5 = 348,
	K_EControllerActionOrigin_SteamDeck_DPad_Move = 349,
	K_EControllerActionOrigin_SteamDeck_DPad_North = 350,
	K_EControllerActionOrigin_SteamDeck_DPad_South = 351,
	K_EControllerActionOrigin_SteamDeck_DPad_West = 352,
	K_EControllerActionOrigin_SteamDeck_DPad_East = 353,
	K_EControllerActionOrigin_SteamDeck_Gyro_Move = 354,
	K_EControllerActionOrigin_SteamDeck_Gyro_Pitch = 355,
	K_EControllerActionOrigin_SteamDeck_Gyro_Yaw = 356,
	K_EControllerActionOrigin_SteamDeck_Gyro_Roll = 357,
	K_EControllerActionOrigin_SteamDeck_Reserved1 = 358,
	K_EControllerActionOrigin_SteamDeck_Reserved2 = 359,
	K_EControllerActionOrigin_SteamDeck_Reserved3 = 360,
	K_EControllerActionOrigin_SteamDeck_Reserved4 = 361,
	K_EControllerActionOrigin_SteamDeck_Reserved5 = 362,
	K_EControllerActionOrigin_SteamDeck_Reserved6 = 363,
	K_EControllerActionOrigin_SteamDeck_Reserved7 = 364,
	K_EControllerActionOrigin_SteamDeck_Reserved8 = 365,
	K_EControllerActionOrigin_SteamDeck_Reserved9 = 366,
	K_EControllerActionOrigin_SteamDeck_Reserved10 = 367,
	K_EControllerActionOrigin_SteamDeck_Reserved11 = 368,
	K_EControllerActionOrigin_SteamDeck_Reserved12 = 369,
	K_EControllerActionOrigin_SteamDeck_Reserved13 = 370,
	K_EControllerActionOrigin_SteamDeck_Reserved14 = 371,
	K_EControllerActionOrigin_SteamDeck_Reserved15 = 372,
	K_EControllerActionOrigin_SteamDeck_Reserved16 = 373,
	K_EControllerActionOrigin_SteamDeck_Reserved17 = 374,
	K_EControllerActionOrigin_SteamDeck_Reserved18 = 375,
	K_EControllerActionOrigin_SteamDeck_Reserved19 = 376,
	K_EControllerActionOrigin_SteamDeck_Reserved20 = 377,
	K_EControllerActionOrigin_Switch_JoyConButton_N = 378,
	K_EControllerActionOrigin_Switch_JoyConButton_E = 379,
	K_EControllerActionOrigin_Switch_JoyConButton_S = 380,
	K_EControllerActionOrigin_Switch_JoyConButton_W = 381,
	K_EControllerActionOrigin_PS5_LeftGrip = 382,
	K_EControllerActionOrigin_PS5_RightGrip = 383,
	K_EControllerActionOrigin_PS5_LeftFn = 384,
	K_EControllerActionOrigin_PS5_RightFn = 385,
	K_EControllerActionOrigin_Count = 386,
	K_EControllerActionOrigin_MaximumPossibleValue = 32767,
 };
Q_ENUM(EControllerActionOrigin)

enum class ESteamControllerLEDFlag
{	
	K_ESteamControllerLEDFlag_SetColor = 0,
	K_ESteamControllerLEDFlag_RestoreUserDefault = 1,
 };
Q_ENUM(ESteamControllerLEDFlag)

enum class EUGCMatchingUGCType
{	
	K_EUGCMatchingUGCType_Items = 0,
	K_EUGCMatchingUGCType_Items_Mtx = 1,
	K_EUGCMatchingUGCType_Items_ReadyToUse = 2,
	K_EUGCMatchingUGCType_Collections = 3,
	K_EUGCMatchingUGCType_Artwork = 4,
	K_EUGCMatchingUGCType_Videos = 5,
	K_EUGCMatchingUGCType_Screenshots = 6,
	K_EUGCMatchingUGCType_AllGuides = 7,
	K_EUGCMatchingUGCType_WebGuides = 8,
	K_EUGCMatchingUGCType_IntegratedGuides = 9,
	K_EUGCMatchingUGCType_UsableInGame = 10,
	K_EUGCMatchingUGCType_ControllerBindings = 11,
	K_EUGCMatchingUGCType_GameManagedItems = 12,
	K_EUGCMatchingUGCType_All = -1,
 };
Q_ENUM(EUGCMatchingUGCType)

enum class EUserUGCList
{	
	K_EUserUGCList_Published = 0,
	K_EUserUGCList_VotedOn = 1,
	K_EUserUGCList_VotedUp = 2,
	K_EUserUGCList_VotedDown = 3,
	K_EUserUGCList_WillVoteLater = 4,
	K_EUserUGCList_Favorited = 5,
	K_EUserUGCList_Subscribed = 6,
	K_EUserUGCList_UsedOrPlayed = 7,
	K_EUserUGCList_Followed = 8,
 };
Q_ENUM(EUserUGCList)

enum class EUserUGCListSortOrder
{	
	K_EUserUGCListSortOrder_CreationOrderDesc = 0,
	K_EUserUGCListSortOrder_CreationOrderAsc = 1,
	K_EUserUGCListSortOrder_TitleAsc = 2,
	K_EUserUGCListSortOrder_LastUpdatedDesc = 3,
	K_EUserUGCListSortOrder_SubscriptionDateDesc = 4,
	K_EUserUGCListSortOrder_VoteScoreDesc = 5,
	K_EUserUGCListSortOrder_ForModeration = 6,
 };
Q_ENUM(EUserUGCListSortOrder)

enum class EUGCQuery
{	
	K_EUGCQuery_RankedByVote = 0,
	K_EUGCQuery_RankedByPublicationDate = 1,
	K_EUGCQuery_AcceptedForGameRankedByAcceptanceDate = 2,
	K_EUGCQuery_RankedByTrend = 3,
	K_EUGCQuery_FavoritedByFriendsRankedByPublicationDate = 4,
	K_EUGCQuery_CreatedByFriendsRankedByPublicationDate = 5,
	K_EUGCQuery_RankedByNumTimesReported = 6,
	K_EUGCQuery_CreatedByFollowedUsersRankedByPublicationDate = 7,
	K_EUGCQuery_NotYetRated = 8,
	K_EUGCQuery_RankedByTotalVotesAsc = 9,
	K_EUGCQuery_RankedByVotesUp = 10,
	K_EUGCQuery_RankedByTextSearch = 11,
	K_EUGCQuery_RankedByTotalUniqueSubscriptions = 12,
	K_EUGCQuery_RankedByPlaytimeTrend = 13,
	K_EUGCQuery_RankedByTotalPlaytime = 14,
	K_EUGCQuery_RankedByAveragePlaytimeTrend = 15,
	K_EUGCQuery_RankedByLifetimeAveragePlaytime = 16,
	K_EUGCQuery_RankedByPlaytimeSessionsTrend = 17,
	K_EUGCQuery_RankedByLifetimePlaytimeSessions = 18,
	K_EUGCQuery_RankedByLastUpdatedDate = 19,
 };
Q_ENUM(EUGCQuery)

enum class EItemUpdateStatus
{	
	K_EItemUpdateStatusInvalid = 0,
	K_EItemUpdateStatusPreparingConfig = 1,
	K_EItemUpdateStatusPreparingContent = 2,
	K_EItemUpdateStatusUploadingContent = 3,
	K_EItemUpdateStatusUploadingPreviewFile = 4,
	K_EItemUpdateStatusCommittingChanges = 5,
 };
Q_ENUM(EItemUpdateStatus)

enum class EItemState
{	
	K_EItemStateNone = 0,
	K_EItemStateSubscribed = 1,
	K_EItemStateLegacyItem = 2,
	K_EItemStateInstalled = 4,
	K_EItemStateNeedsUpdate = 8,
	K_EItemStateDownloading = 16,
	K_EItemStateDownloadPending = 32,
 };
Q_ENUM(EItemState)

enum class EItemStatistic
{	
	K_EItemStatistic_NumSubscriptions = 0,
	K_EItemStatistic_NumFavorites = 1,
	K_EItemStatistic_NumFollowers = 2,
	K_EItemStatistic_NumUniqueSubscriptions = 3,
	K_EItemStatistic_NumUniqueFavorites = 4,
	K_EItemStatistic_NumUniqueFollowers = 5,
	K_EItemStatistic_NumUniqueWebsiteViews = 6,
	K_EItemStatistic_ReportScore = 7,
	K_EItemStatistic_NumSecondsPlayed = 8,
	K_EItemStatistic_NumPlaytimeSessions = 9,
	K_EItemStatistic_NumComments = 10,
	K_EItemStatistic_NumSecondsPlayedDuringTimePeriod = 11,
	K_EItemStatistic_NumPlaytimeSessionsDuringTimePeriod = 12,
 };
Q_ENUM(EItemStatistic)

enum class EItemPreviewType
{	
	K_EItemPreviewType_Image = 0,
	K_EItemPreviewType_YouTubeVideo = 1,
	K_EItemPreviewType_Sketchfab = 2,
	K_EItemPreviewType_EnvironmentMap_HorizontalCross = 3,
	K_EItemPreviewType_EnvironmentMap_LatLong = 4,
	K_EItemPreviewType_ReservedMax = 255,
 };
Q_ENUM(EItemPreviewType)

enum class EUGCContentDescriptorID
{	
	K_EUGCContentDescriptor_NudityOrSexualContent = 1,
	K_EUGCContentDescriptor_FrequentViolenceOrGore = 2,
	K_EUGCContentDescriptor_AdultOnlySexualContent = 3,
	K_EUGCContentDescriptor_GratuitousSexualContent = 4,
	K_EUGCContentDescriptor_AnyMatureContent = 5,
 };
Q_ENUM(EUGCContentDescriptorID)

enum class ESteamItemFlags
{	
	K_ESteamItemNoTrade = 1,
	K_ESteamItemRemoved = 256,
	K_ESteamItemConsumed = 512,
 };
Q_ENUM(ESteamItemFlags)

enum class EParentalFeature
{	
	K_EFeatureInvalid = 0,
	K_EFeatureStore = 1,
	K_EFeatureCommunity = 2,
	K_EFeatureProfile = 3,
	K_EFeatureFriends = 4,
	K_EFeatureNews = 5,
	K_EFeatureTrading = 6,
	K_EFeatureSettings = 7,
	K_EFeatureConsole = 8,
	K_EFeatureBrowser = 9,
	K_EFeatureParentalSetup = 10,
	K_EFeatureLibrary = 11,
	K_EFeatureTest = 12,
	K_EFeatureSiteLicense = 13,
	K_EFeatureKioskMode = 14,
	K_EFeatureMax = 15,
 };
Q_ENUM(EParentalFeature)

enum class ESteamDeviceFormFactor
{	
	K_ESteamDeviceFormFactorUnknown = 0,
	K_ESteamDeviceFormFactorPhone = 1,
	K_ESteamDeviceFormFactorTablet = 2,
	K_ESteamDeviceFormFactorComputer = 3,
	K_ESteamDeviceFormFactorTV = 4,
 };
Q_ENUM(ESteamDeviceFormFactor)

enum class ESteamNetworkingAvailability
{	
	K_ESteamNetworkingAvailability_CannotTry = -102,
	K_ESteamNetworkingAvailability_Failed = -101,
	K_ESteamNetworkingAvailability_Previously = -100,
	K_ESteamNetworkingAvailability_Retrying = -10,
	K_ESteamNetworkingAvailability_NeverTried = 1,
	K_ESteamNetworkingAvailability_Waiting = 2,
	K_ESteamNetworkingAvailability_Attempting = 3,
	K_ESteamNetworkingAvailability_Current = 100,
	K_ESteamNetworkingAvailability_Unknown = 0,
	K_ESteamNetworkingAvailability__Force32bit = 2147483647,
 };
Q_ENUM(ESteamNetworkingAvailability)

enum class ESteamNetworkingIdentityType
{	
	K_ESteamNetworkingIdentityType_Invalid = 0,
	K_ESteamNetworkingIdentityType_SteamID = 16,
	K_ESteamNetworkingIdentityType_XboxPairwiseID = 17,
	K_ESteamNetworkingIdentityType_SonyPSN = 18,
	K_ESteamNetworkingIdentityType_GoogleStadia = 19,
	K_ESteamNetworkingIdentityType_IPAddress = 1,
	K_ESteamNetworkingIdentityType_GenericString = 2,
	K_ESteamNetworkingIdentityType_GenericBytes = 3,
	K_ESteamNetworkingIdentityType_UnknownType = 4,
	K_ESteamNetworkingIdentityType__Force32bit = 2147483647,
 };
Q_ENUM(ESteamNetworkingIdentityType)

enum class ESteamNetworkingFakeIPType
{	
	K_ESteamNetworkingFakeIPType_Invalid = 0,
	K_ESteamNetworkingFakeIPType_NotFake = 1,
	K_ESteamNetworkingFakeIPType_GlobalIPv4 = 2,
	K_ESteamNetworkingFakeIPType_LocalIPv4 = 3,
	K_ESteamNetworkingFakeIPType__Force32Bit = 2147483647,
 };
Q_ENUM(ESteamNetworkingFakeIPType)

enum class ESteamNetworkingConnectionState
{	
	K_ESteamNetworkingConnectionState_None = 0,
	K_ESteamNetworkingConnectionState_Connecting = 1,
	K_ESteamNetworkingConnectionState_FindingRoute = 2,
	K_ESteamNetworkingConnectionState_Connected = 3,
	K_ESteamNetworkingConnectionState_ClosedByPeer = 4,
	K_ESteamNetworkingConnectionState_ProblemDetectedLocally = 5,
	K_ESteamNetworkingConnectionState_FinWait = -1,
	K_ESteamNetworkingConnectionState_Linger = -2,
	K_ESteamNetworkingConnectionState_Dead = -3,
	K_ESteamNetworkingConnectionState__Force32Bit = 2147483647,
 };
Q_ENUM(ESteamNetworkingConnectionState)

enum class ESteamNetConnectionEnd
{	
	K_ESteamNetConnectionEnd_Invalid = 0,
	K_ESteamNetConnectionEnd_App_Min = 1000,
	K_ESteamNetConnectionEnd_App_Generic = 1000,
	K_ESteamNetConnectionEnd_App_Max = 1999,
	K_ESteamNetConnectionEnd_AppException_Min = 2000,
	K_ESteamNetConnectionEnd_AppException_Generic = 2000,
	K_ESteamNetConnectionEnd_AppException_Max = 2999,
	K_ESteamNetConnectionEnd_Local_Min = 3000,
	K_ESteamNetConnectionEnd_Local_OfflineMode = 3001,
	K_ESteamNetConnectionEnd_Local_ManyRelayConnectivity = 3002,
	K_ESteamNetConnectionEnd_Local_HostedServerPrimaryRelay = 3003,
	K_ESteamNetConnectionEnd_Local_NetworkConfig = 3004,
	K_ESteamNetConnectionEnd_Local_Rights = 3005,
	K_ESteamNetConnectionEnd_Local_P2P_ICE_NoPublicAddresses = 3006,
	K_ESteamNetConnectionEnd_Local_Max = 3999,
	K_ESteamNetConnectionEnd_Remote_Min = 4000,
	K_ESteamNetConnectionEnd_Remote_Timeout = 4001,
	K_ESteamNetConnectionEnd_Remote_BadCrypt = 4002,
	K_ESteamNetConnectionEnd_Remote_BadCert = 4003,
	K_ESteamNetConnectionEnd_Remote_BadProtocolVersion = 4006,
	K_ESteamNetConnectionEnd_Remote_P2P_ICE_NoPublicAddresses = 4007,
	K_ESteamNetConnectionEnd_Remote_Max = 4999,
	K_ESteamNetConnectionEnd_Misc_Min = 5000,
	K_ESteamNetConnectionEnd_Misc_Generic = 5001,
	K_ESteamNetConnectionEnd_Misc_InternalError = 5002,
	K_ESteamNetConnectionEnd_Misc_Timeout = 5003,
	K_ESteamNetConnectionEnd_Misc_SteamConnectivity = 5005,
	K_ESteamNetConnectionEnd_Misc_NoRelaySessionsToClient = 5006,
	K_ESteamNetConnectionEnd_Misc_P2P_Rendezvous = 5008,
	K_ESteamNetConnectionEnd_Misc_P2P_NAT_Firewall = 5009,
	K_ESteamNetConnectionEnd_Misc_PeerSentNoConnection = 5010,
	K_ESteamNetConnectionEnd_Misc_Max = 5999,
	K_ESteamNetConnectionEnd__Force32Bit = 2147483647,
 };
Q_ENUM(ESteamNetConnectionEnd)

enum class ESteamNetworkingConfigScope
{	
	K_ESteamNetworkingConfig_Global = 1,
	K_ESteamNetworkingConfig_SocketsInterface = 2,
	K_ESteamNetworkingConfig_ListenSocket = 3,
	K_ESteamNetworkingConfig_Connection = 4,
	K_ESteamNetworkingConfigScope__Force32Bit = 2147483647,
 };
Q_ENUM(ESteamNetworkingConfigScope)

enum class ESteamNetworkingConfigDataType
{	
	K_ESteamNetworkingConfig_Int32 = 1,
	K_ESteamNetworkingConfig_Int64 = 2,
	K_ESteamNetworkingConfig_Float = 3,
	K_ESteamNetworkingConfig_String = 4,
	K_ESteamNetworkingConfig_Ptr = 5,
	K_ESteamNetworkingConfigDataType__Force32Bit = 2147483647,
 };
Q_ENUM(ESteamNetworkingConfigDataType)

enum class ESteamNetworkingConfigValue
{	
	K_ESteamNetworkingConfig_Invalid = 0,
	K_ESteamNetworkingConfig_TimeoutInitial = 24,
	K_ESteamNetworkingConfig_TimeoutConnected = 25,
	K_ESteamNetworkingConfig_SendBufferSize = 9,
	K_ESteamNetworkingConfig_ConnectionUserData = 40,
	K_ESteamNetworkingConfig_SendRateMin = 10,
	K_ESteamNetworkingConfig_SendRateMax = 11,
	K_ESteamNetworkingConfig_NagleTime = 12,
	K_ESteamNetworkingConfig_IP_AllowWithoutAuth = 23,
	K_ESteamNetworkingConfig_MTU_PacketSize = 32,
	K_ESteamNetworkingConfig_MTU_DataSize = 33,
	K_ESteamNetworkingConfig_Unencrypted = 34,
	K_ESteamNetworkingConfig_SymmetricConnect = 37,
	K_ESteamNetworkingConfig_LocalVirtualPort = 38,
	K_ESteamNetworkingConfig_DualWifi_Enable = 39,
	K_ESteamNetworkingConfig_EnableDiagnosticsUI = 46,
	K_ESteamNetworkingConfig_FakePacketLoss_Send = 2,
	K_ESteamNetworkingConfig_FakePacketLoss_Recv = 3,
	K_ESteamNetworkingConfig_FakePacketLag_Send = 4,
	K_ESteamNetworkingConfig_FakePacketLag_Recv = 5,
	K_ESteamNetworkingConfig_FakePacketReorder_Send = 6,
	K_ESteamNetworkingConfig_FakePacketReorder_Recv = 7,
	K_ESteamNetworkingConfig_FakePacketReorder_Time = 8,
	K_ESteamNetworkingConfig_FakePacketDup_Send = 26,
	K_ESteamNetworkingConfig_FakePacketDup_Recv = 27,
	K_ESteamNetworkingConfig_FakePacketDup_TimeMax = 28,
	K_ESteamNetworkingConfig_PacketTraceMaxBytes = 41,
	K_ESteamNetworkingConfig_FakeRateLimit_Send_Rate = 42,
	K_ESteamNetworkingConfig_FakeRateLimit_Send_Burst = 43,
	K_ESteamNetworkingConfig_FakeRateLimit_Recv_Rate = 44,
	K_ESteamNetworkingConfig_FakeRateLimit_Recv_Burst = 45,
	K_ESteamNetworkingConfig_Callback_ConnectionStatusChanged = 201,
	K_ESteamNetworkingConfig_Callback_AuthStatusChanged = 202,
	K_ESteamNetworkingConfig_Callback_RelayNetworkStatusChanged = 203,
	K_ESteamNetworkingConfig_Callback_MessagesSessionRequest = 204,
	K_ESteamNetworkingConfig_Callback_MessagesSessionFailed = 205,
	K_ESteamNetworkingConfig_Callback_CreateConnectionSignaling = 206,
	K_ESteamNetworkingConfig_Callback_FakeIPResult = 207,
	K_ESteamNetworkingConfig_P2P_STUN_ServerList = 103,
	K_ESteamNetworkingConfig_P2P_Transport_ICE_Enable = 104,
	K_ESteamNetworkingConfig_P2P_Transport_ICE_Penalty = 105,
	K_ESteamNetworkingConfig_P2P_Transport_SDR_Penalty = 106,
	K_ESteamNetworkingConfig_P2P_TURN_ServerList = 107,
	K_ESteamNetworkingConfig_P2P_TURN_UserList = 108,
	K_ESteamNetworkingConfig_P2P_TURN_PassList = 109,
	K_ESteamNetworkingConfig_P2P_Transport_ICE_Implementation = 110,
	K_ESteamNetworkingConfig_SDRClient_ConsecutitivePingTimeoutsFailInitial = 19,
	K_ESteamNetworkingConfig_SDRClient_ConsecutitivePingTimeoutsFail = 20,
	K_ESteamNetworkingConfig_SDRClient_MinPingsBeforePingAccurate = 21,
	K_ESteamNetworkingConfig_SDRClient_SingleSocket = 22,
	K_ESteamNetworkingConfig_SDRClient_ForceRelayCluster = 29,
	K_ESteamNetworkingConfig_SDRClient_DebugTicketAddress = 30,
	K_ESteamNetworkingConfig_SDRClient_ForceProxyAddr = 31,
	K_ESteamNetworkingConfig_SDRClient_FakeClusterPing = 36,
	K_ESteamNetworkingConfig_LogLevel_AckRTT = 13,
	K_ESteamNetworkingConfig_LogLevel_PacketDecode = 14,
	K_ESteamNetworkingConfig_LogLevel_Message = 15,
	K_ESteamNetworkingConfig_LogLevel_PacketGaps = 16,
	K_ESteamNetworkingConfig_LogLevel_P2PRendezvous = 17,
	K_ESteamNetworkingConfig_LogLevel_SDRRelayPings = 18,
	K_ESteamNetworkingConfig_DELETED_EnumerateDevVars = 35,
	K_ESteamNetworkingConfigValue__Force32Bit = 2147483647,
 };
Q_ENUM(ESteamNetworkingConfigValue)

enum class ESteamNetworkingGetConfigValueResult
{	
	K_ESteamNetworkingGetConfigValue_BadValue = -1,
	K_ESteamNetworkingGetConfigValue_BadScopeObj = -2,
	K_ESteamNetworkingGetConfigValue_BufferTooSmall = -3,
	K_ESteamNetworkingGetConfigValue_OK = 1,
	K_ESteamNetworkingGetConfigValue_OKInherited = 2,
	K_ESteamNetworkingGetConfigValueResult__Force32Bit = 2147483647,
 };
Q_ENUM(ESteamNetworkingGetConfigValueResult)

enum class ESteamNetworkingSocketsDebugOutputType
{	
	K_ESteamNetworkingSocketsDebugOutputType_None = 0,
	K_ESteamNetworkingSocketsDebugOutputType_Bug = 1,
	K_ESteamNetworkingSocketsDebugOutputType_Error = 2,
	K_ESteamNetworkingSocketsDebugOutputType_Important = 3,
	K_ESteamNetworkingSocketsDebugOutputType_Warning = 4,
	K_ESteamNetworkingSocketsDebugOutputType_Msg = 5,
	K_ESteamNetworkingSocketsDebugOutputType_Verbose = 6,
	K_ESteamNetworkingSocketsDebugOutputType_Debug = 7,
	K_ESteamNetworkingSocketsDebugOutputType_Everything = 8,
	K_ESteamNetworkingSocketsDebugOutputType__Force32Bit = 2147483647,
 };
Q_ENUM(ESteamNetworkingSocketsDebugOutputType)

enum class EServerMode
{	
	EServerModeInvalid = 0,
	EServerModeNoAuthentication = 1,
	EServerModeAuthentication = 2,
	EServerModeAuthenticationAndSecure = 3,
 };
Q_ENUM(EServerMode)


};
}
    