project(SteamSDK LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 COMPONENTS Core)

set(HEADER
    public/steam/isteamapplist.h
    public/steam/isteamapps.h
    public/steam/isteamappticket.h
    public/steam/isteamclient.h
    public/steam/isteamcontroller.h
    public/steam/isteamfriends.h
    public/steam/isteamgamecoordinator.h
    public/steam/isteamgameserver.h
    public/steam/isteamgameserverstats.h
    public/steam/isteamhtmlsurface.h
    public/steam/isteamhttp.h
    public/steam/isteaminput.h
    public/steam/isteaminventory.h
    public/steam/isteammatchmaking.h
    public/steam/isteammusic.h
    public/steam/isteammusicremote.h
    public/steam/isteamnetworking.h
    public/steam/isteamnetworkingmessages.h
    public/steam/isteamnetworkingsockets.h
    public/steam/isteamnetworkingutils.h
    public/steam/isteamparentalsettings.h
    public/steam/isteamps3overlayrenderer.h
    public/steam/isteamremoteplay.h
    public/steam/isteamremotestorage.h
    public/steam/isteamscreenshots.h
    public/steam/isteamugc.h
    public/steam/isteamuser.h
    public/steam/isteamuserstats.h
    public/steam/isteamutils.h
    public/steam/isteamvideo.h
    public/steam/matchmakingtypes.h
    public/steam/steam_api_common.h
    public/steam/steam_api_flat.h
    public/steam/steam_api_internal.h
    public/steam/steam_api.h
    public/steam/steam_gameserver.h
    public/steam/steamclientpublic.h
    public/steam/steamencryptedappticket.h
    public/steam/steamhttpenums.h
    public/steam/steamnetworkingtypes.h
    public/steam/steamps3params.h
    public/steam/steamtypes.h
    public/steam/steamuniverse.h)

if(${SCREENPLAY_STEAM})
    add_library(${PROJECT_NAME} STATIC ${HEADER})
    target_include_directories(${PROJECT_NAME} PUBLIC public/)
    target_link_libraries(${PROJECT_NAME})
endif()

# We allaways need the generated enums as a workaround to register these enums in app.cpp. Registering in the ScreenPlayWorkshop plugin does
# not work for some reason.
add_library(SteamSDKQtEnums STATIC public/steam/steam_qt_enums_generated.h)
target_include_directories(SteamSDKQtEnums PUBLIC public/)
target_link_libraries(SteamSDKQtEnums PRIVATE Qt6::Core)
