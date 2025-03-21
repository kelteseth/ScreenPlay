# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
project(
    ScreenPlayWeather
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LibArchive REQUIRED)
find_package(
    Qt6
    COMPONENTS Core Quick
    REQUIRED)

qt_standard_project_setup(REQUIRES 6.8)

set(QML qml/TestMain.qml)

set(QML_PLUGIN_SOURCES src/screenplayweather.cpp)

set(QML_PLUGIN_HEADER src/day.h src/screenplayweather.h)

set(RESOURCES
    # cmake-format: sort
    assets/icons/wi-alien.svg
    assets/icons/wi-barometer.svg
    assets/icons/wi-celsius.svg
    assets/icons/wi-cloud-down.svg
    assets/icons/wi-cloud-refresh.svg
    assets/icons/wi-cloud-up.svg
    assets/icons/wi-cloud.svg
    assets/icons/wi-cloudy-gusts.svg
    assets/icons/wi-cloudy-windy.svg
    assets/icons/wi-cloudy.svg
    assets/icons/wi-day-cloudy-gusts.svg
    assets/icons/wi-day-cloudy-high.svg
    assets/icons/wi-day-cloudy-windy.svg
    assets/icons/wi-day-cloudy.svg
    assets/icons/wi-day-fog.svg
    assets/icons/wi-day-hail.svg
    assets/icons/wi-day-haze.svg
    assets/icons/wi-day-light-wind.svg
    assets/icons/wi-day-lightning.svg
    assets/icons/wi-day-rain-mix.svg
    assets/icons/wi-day-rain-wind.svg
    assets/icons/wi-day-rain.svg
    assets/icons/wi-day-showers.svg
    assets/icons/wi-day-sleet-storm.svg
    assets/icons/wi-day-sleet.svg
    assets/icons/wi-day-snow-thunderstorm.svg
    assets/icons/wi-day-snow-wind.svg
    assets/icons/wi-day-snow.svg
    assets/icons/wi-day-sprinkle.svg
    assets/icons/wi-day-storm-showers.svg
    assets/icons/wi-day-sunny-overcast.svg
    assets/icons/wi-day-sunny.svg
    assets/icons/wi-day-thunderstorm.svg
    assets/icons/wi-day-windy.svg
    assets/icons/wi-degrees.svg
    assets/icons/wi-direction-down-left.svg
    assets/icons/wi-direction-down-right.svg
    assets/icons/wi-direction-down.svg
    assets/icons/wi-direction-left.svg
    assets/icons/wi-direction-right.svg
    assets/icons/wi-direction-up-left.svg
    assets/icons/wi-direction-up-right.svg
    assets/icons/wi-direction-up.svg
    assets/icons/wi-dust.svg
    assets/icons/wi-earthquake.svg
    assets/icons/wi-fahrenheit.svg
    assets/icons/wi-fire.svg
    assets/icons/wi-flood.svg
    assets/icons/wi-fog.svg
    assets/icons/wi-gale-warning.svg
    assets/icons/wi-hail.svg
    assets/icons/wi-horizon-alt.svg
    assets/icons/wi-horizon.svg
    assets/icons/wi-hot.svg
    assets/icons/wi-humidity.svg
    assets/icons/wi-hurricane-warning.svg
    assets/icons/wi-hurricane.svg
    assets/icons/wi-lightning.svg
    assets/icons/wi-lunar-eclipse.svg
    assets/icons/wi-meteor.svg
    assets/icons/wi-moon-alt-first-quarter.svg
    assets/icons/wi-moon-alt-full.svg
    assets/icons/wi-moon-alt-new.svg
    assets/icons/wi-moon-alt-third-quarter.svg
    assets/icons/wi-moon-alt-waning-crescent-1.svg
    assets/icons/wi-moon-alt-waning-crescent-2.svg
    assets/icons/wi-moon-alt-waning-crescent-3.svg
    assets/icons/wi-moon-alt-waning-crescent-4.svg
    assets/icons/wi-moon-alt-waning-crescent-5.svg
    assets/icons/wi-moon-alt-waning-crescent-6.svg
    assets/icons/wi-moon-alt-waning-gibbous-1.svg
    assets/icons/wi-moon-alt-waning-gibbous-2.svg
    assets/icons/wi-moon-alt-waning-gibbous-3.svg
    assets/icons/wi-moon-alt-waning-gibbous-4.svg
    assets/icons/wi-moon-alt-waning-gibbous-5.svg
    assets/icons/wi-moon-alt-waning-gibbous-6.svg
    assets/icons/wi-moon-alt-waxing-crescent-1.svg
    assets/icons/wi-moon-alt-waxing-crescent-2.svg
    assets/icons/wi-moon-alt-waxing-crescent-3.svg
    assets/icons/wi-moon-alt-waxing-crescent-4.svg
    assets/icons/wi-moon-alt-waxing-crescent-5.svg
    assets/icons/wi-moon-alt-waxing-crescent-6.svg
    assets/icons/wi-moon-alt-waxing-gibbous-1.svg
    assets/icons/wi-moon-alt-waxing-gibbous-2.svg
    assets/icons/wi-moon-alt-waxing-gibbous-3.svg
    assets/icons/wi-moon-alt-waxing-gibbous-4.svg
    assets/icons/wi-moon-alt-waxing-gibbous-5.svg
    assets/icons/wi-moon-alt-waxing-gibbous-6.svg
    assets/icons/wi-moon-first-quarter.svg
    assets/icons/wi-moon-full.svg
    assets/icons/wi-moon-new.svg
    assets/icons/wi-moon-third-quarter.svg
    assets/icons/wi-moon-waning-crescent-1.svg
    assets/icons/wi-moon-waning-crescent-2.svg
    assets/icons/wi-moon-waning-crescent-3.svg
    assets/icons/wi-moon-waning-crescent-4.svg
    assets/icons/wi-moon-waning-crescent-5.svg
    assets/icons/wi-moon-waning-crescent-6.svg
    assets/icons/wi-moon-waning-gibbous-1.svg
    assets/icons/wi-moon-waning-gibbous-2.svg
    assets/icons/wi-moon-waning-gibbous-3.svg
    assets/icons/wi-moon-waning-gibbous-4.svg
    assets/icons/wi-moon-waning-gibbous-5.svg
    assets/icons/wi-moon-waning-gibbous-6.svg
    assets/icons/wi-moon-waxing-6.svg
    assets/icons/wi-moon-waxing-crescent-1.svg
    assets/icons/wi-moon-waxing-crescent-2.svg
    assets/icons/wi-moon-waxing-crescent-3.svg
    assets/icons/wi-moon-waxing-crescent-4.svg
    assets/icons/wi-moon-waxing-crescent-5.svg
    assets/icons/wi-moon-waxing-gibbous-1.svg
    assets/icons/wi-moon-waxing-gibbous-2.svg
    assets/icons/wi-moon-waxing-gibbous-3.svg
    assets/icons/wi-moon-waxing-gibbous-4.svg
    assets/icons/wi-moon-waxing-gibbous-5.svg
    assets/icons/wi-moon-waxing-gibbous-6.svg
    assets/icons/wi-moonrise.svg
    assets/icons/wi-moonset.svg
    assets/icons/wi-na.svg
    assets/icons/wi-night-alt-cloudy-gusts.svg
    assets/icons/wi-night-alt-cloudy-high.svg
    assets/icons/wi-night-alt-cloudy-windy.svg
    assets/icons/wi-night-alt-cloudy.svg
    assets/icons/wi-night-alt-hail.svg
    assets/icons/wi-night-alt-lightning.svg
    assets/icons/wi-night-alt-partly-cloudy.svg
    assets/icons/wi-night-alt-rain-mix.svg
    assets/icons/wi-night-alt-rain-wind.svg
    assets/icons/wi-night-alt-rain.svg
    assets/icons/wi-night-alt-showers.svg
    assets/icons/wi-night-alt-sleet-storm.svg
    assets/icons/wi-night-alt-sleet.svg
    assets/icons/wi-night-alt-snow-thunderstorm.svg
    assets/icons/wi-night-alt-snow-wind.svg
    assets/icons/wi-night-alt-snow.svg
    assets/icons/wi-night-alt-sprinkle.svg
    assets/icons/wi-night-alt-storm-showers.svg
    assets/icons/wi-night-alt-thunderstorm.svg
    assets/icons/wi-night-clear.svg
    assets/icons/wi-night-cloudy-gusts.svg
    assets/icons/wi-night-cloudy-high.svg
    assets/icons/wi-night-cloudy-windy.svg
    assets/icons/wi-night-cloudy.svg
    assets/icons/wi-night-fog.svg
    assets/icons/wi-night-hail.svg
    assets/icons/wi-night-lightning.svg
    assets/icons/wi-night-partly-cloudy.svg
    assets/icons/wi-night-rain-mix.svg
    assets/icons/wi-night-rain-wind.svg
    assets/icons/wi-night-rain.svg
    assets/icons/wi-night-showers.svg
    assets/icons/wi-night-sleet-storm.svg
    assets/icons/wi-night-sleet.svg
    assets/icons/wi-night-snow-thunderstorm.svg
    assets/icons/wi-night-snow-wind.svg
    assets/icons/wi-night-snow.svg
    assets/icons/wi-night-sprinkle.svg
    assets/icons/wi-night-storm-showers.svg
    assets/icons/wi-night-thunderstorm.svg
    assets/icons/wi-rain-mix.svg
    assets/icons/wi-rain-wind.svg
    assets/icons/wi-rain.svg
    assets/icons/wi-raindrop.svg
    assets/icons/wi-raindrops.svg
    assets/icons/wi-refresh-alt.svg
    assets/icons/wi-refresh.svg
    assets/icons/wi-sandstorm.svg
    assets/icons/wi-showers.svg
    assets/icons/wi-sleet.svg
    assets/icons/wi-small-craft-advisory.svg
    assets/icons/wi-smog.svg
    assets/icons/wi-smoke.svg
    assets/icons/wi-snow-wind.svg
    assets/icons/wi-snow.svg
    assets/icons/wi-snowflake-cold.svg
    assets/icons/wi-solar-eclipse.svg
    assets/icons/wi-sprinkle.svg
    assets/icons/wi-stars.svg
    assets/icons/wi-storm-showers.svg
    assets/icons/wi-storm-warning.svg
    assets/icons/wi-strong-wind.svg
    assets/icons/wi-sunrise.svg
    assets/icons/wi-sunset.svg
    assets/icons/wi-thermometer-exterior.svg
    assets/icons/wi-thermometer-internal.svg
    assets/icons/wi-thermometer.svg
    assets/icons/wi-thunderstorm.svg
    assets/icons/wi-time-1.svg
    assets/icons/wi-time-10.svg
    assets/icons/wi-time-11.svg
    assets/icons/wi-time-12.svg
    assets/icons/wi-time-2.svg
    assets/icons/wi-time-3.svg
    assets/icons/wi-time-4.svg
    assets/icons/wi-time-5.svg
    assets/icons/wi-time-6.svg
    assets/icons/wi-time-7.svg
    assets/icons/wi-time-8.svg
    assets/icons/wi-time-9.svg
    assets/icons/wi-tornado.svg
    assets/icons/wi-train.svg
    assets/icons/wi-tsunami.svg
    assets/icons/wi-umbrella.svg
    assets/icons/wi-volcano.svg
    assets/icons/wi-wind-beaufort-0.svg
    assets/icons/wi-wind-beaufort-1.svg
    assets/icons/wi-wind-beaufort-10.svg
    assets/icons/wi-wind-beaufort-11.svg
    assets/icons/wi-wind-beaufort-12.svg
    assets/icons/wi-wind-beaufort-2.svg
    assets/icons/wi-wind-beaufort-3.svg
    assets/icons/wi-wind-beaufort-4.svg
    assets/icons/wi-wind-beaufort-5.svg
    assets/icons/wi-wind-beaufort-6.svg
    assets/icons/wi-wind-beaufort-7.svg
    assets/icons/wi-wind-beaufort-8.svg
    assets/icons/wi-wind-beaufort-9.svg
    assets/icons/wi-wind-deg.svg
    assets/icons/wi-windy.svg)

qt_add_library(ScreenPlayWeatherLib STATIC)
target_compile_features(ScreenPlayWeatherLib PRIVATE cxx_std_23)
target_link_libraries(ScreenPlayWeatherLib PUBLIC ScreenPlayCoreLib ScreenPlayCoreLibplugin)
target_include_directories(ScreenPlayWeatherLib PUBLIC src/)

qt_add_qml_module(
    ScreenPlayWeatherLib
    URI
    ScreenPlayWeather
    VERSION
    1.0
    QML_FILES
    ${QML}
    SOURCES
    ${QML_PLUGIN_SOURCES}
    ${QML_PLUGIN_HEADER}
    RESOURCES
    ${RESOURCES})

if(${SCREENPLAY_TESTS})
    qt_add_executable(tst_ScreenPlayWeather src/TestMain.cpp)
    target_link_libraries(
        tst_ScreenPlayWeather
        PRIVATE Qt6::Quick
                ScreenPlayWeatherLib
                ScreenPlayWeatherLibplugin
                ScreenPlayCoreLib
                ScreenPlayCoreLibplugin)
endif()
