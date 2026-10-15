cmake_minimum_required(VERSION 3.22)

project(whitelist-tray VERSION 6.7.0.1)

set(QT_MIN_VERSION "6.8.0")
set(KF6_MIN_VERSION "6.10.0")
set(PLASMA_MIN_VERSION "6.3.0")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(KDE_COMPILERSETTINGS_LEVEL "6.0")

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMQtDeclareLoggingCategory)
include(FeatureSummary)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS DBus Gui Quick Widgets)
find_package(Qt6GuiPrivate ${QT_MIN_VERSION} CONFIG QUIET)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    Config
    I18n
    IconThemes
    ItemModels
    JobWidgets
    KIO
    Service
    StatusNotifierItem
    WindowSystem
    XmlGui
)

find_package(Plasma ${PLASMA_MIN_VERSION} REQUIRED)
find_package(PlasmaQuick ${PLASMA_MIN_VERSION} REQUIRED)

option(WITH_X11 "Build with X11 support" ON)
if(WITH_X11)
    set(HAVE_X11 1)
else()
    set(HAVE_X11 0)
endif()
configure_file(src/config-X11.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-X11.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Private copies: keep symbols out of the global scope so this widget can be
# loaded in the same plasmashell as the stock system tray.
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

add_subdirectory(third_party/libdbusmenuqt)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/libdbusmenuqt)
add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(autotests)
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
