cmake_minimum_required(VERSION 3.22)

project(prefixdev-runner VERSION 0.1.0)

set(QT_MIN_VERSION "6.6.0")
set(KF6_MIN_VERSION "6.0.0")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The runner is loaded into the system krunner/plasmashell, so it must link
# the distro's Qt and KF libraries, never ones from a pixi/conda env.
if(DEFINED ENV{CONDA_PREFIX})
    list(APPEND CMAKE_IGNORE_PREFIX_PATH "$ENV{CONDA_PREFIX}")
endif()

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Gui Network)
find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS Runner I18n CoreAddons)

add_definitions(-DTRANSLATION_DOMAIN=\"krunner_prefixdev\")

add_library(prefixdevcore STATIC src/prefixdev.cpp)
set_property(TARGET prefixdevcore PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(prefixdevcore PUBLIC src)
target_link_libraries(prefixdevcore PUBLIC Qt6::Core)

kcoreaddons_add_plugin(krunner_prefixdev SOURCES src/prefixdevrunner.cpp INSTALL_NAMESPACE "kf6/krunner")
target_link_libraries(krunner_prefixdev
    prefixdevcore
    Qt6::Gui
    Qt6::Network
    KF6::Runner
    KF6::I18n
)

if(BUILD_TESTING)
    find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Test)
    add_subdirectory(autotests)
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
