
plasma_install_package(package org.kde.plasma.whitelisttray)

set(systemtraycontainer_SRCS
    systemtraycontainer.cpp
    systemtraycontainer.h
)

ecm_qt_declare_logging_category(systemtraycontainer_SRCS HEADER debug.h
                                            IDENTIFIER SYSTEM_TRAY_CONTAINER
                                            CATEGORY_NAME kde.whitelisttraycontainer
                                            DEFAULT_SEVERITY Info)

kcoreaddons_add_plugin(org.kde.plasma.whitelisttray SOURCES ${systemtraycontainer_SRCS} INSTALL_NAMESPACE "plasma/applets")

target_link_libraries(org.kde.plasma.whitelisttray
                      Qt::Gui
                      Qt::Quick
                      KF5::Plasma
                      KF5::XmlGui
                      KF5::I18n)
