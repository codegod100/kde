cmake_minimum_required(VERSION 3.16)

# Plasma 5 build of the whitelist tray, for distros still on Plasma 5.27
# (Ubuntu/Kubuntu 24.04, Debian 12). Forked from plasma-workspace
# applets/systemtray, Plasma/5.27 branch. The Plasma 6 build is in ../.
project(whitelist-tray-plasma5 VERSION 5.27.0.1)

set(QT_MIN_VERSION "5.15.2")
set(KF5_MIN_VERSION "5.102.0")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(KDE_COMPILERSETTINGS_LEVEL "5.82")

find_package(Qt5Core ${QT_MIN_VERSION} CONFIG QUIET)
if(NOT Qt5Core_FOUND)
    message(FATAL_ERROR
        "Qt ${QT_MIN_VERSION} or newer (Qt 5) development files were not found. "
        "Qt 5, KDE Frameworks 5 and plasma-framework must come from your distro, matching "
        "the Plasma 5 you run; install the packages listed in ../README.md.")
endif()

# ECM 6 (as shipped by pixi) defaults to Qt 6; pin Qt 5.
set(QT_MAJOR_VERSION 5)
find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMQtDeclareLoggingCategory)
include(FeatureSummary)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS DBus Gui Quick Widgets)
find_package(Qt5Gui ${QT_MIN_VERSION} CONFIG REQUIRED Private)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Config
    CoreAddons
    I18n
    IconThemes
    ItemModels
    Notifications
    Plasma
    WindowSystem
    XmlGui
)

# Wayland fractional scaling for tray icons, as in plasma-workspace.
find_package(Qt5WaylandClient ${QT_MIN_VERSION} CONFIG QUIET)
find_package(Wayland 1.15 QUIET COMPONENTS Client)
find_package(WaylandProtocols 1.31 QUIET)
find_package(QtWaylandScanner QUIET)
if(Qt5WaylandClient_FOUND AND Wayland_FOUND AND WaylandProtocols_FOUND AND QtWaylandScanner_FOUND)
    set(HAVE_WaylandProtocols 1)
else()
    set(HAVE_WaylandProtocols 0)
    message(STATUS "Wayland fractional-scale support disabled (needs qtwayland5-private-dev, qtwayland5-dev-tools, libwayland-dev, wayland-protocols)")
endif()

option(WITH_X11 "Build with X11 support" ON)
if(WITH_X11)
    set(HAVE_X11 1)
else()
    set(HAVE_X11 0)
endif()
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/config-X11.h "#define HAVE_X11 ${HAVE_X11}\n")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/config-WaylandProtocols.h "#define HAVE_WaylandProtocols ${HAVE_WaylandProtocols}\n")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Private copy of plasma-workspace's static libdbusmenuqt
add_subdirectory(third_party/libdbusmenuqt)
include_directories(third_party/libdbusmenuqt)

add_subdirectory(src)
add_subdirectory(container)

if(BUILD_TESTING)
    add_subdirectory(autotests)
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
