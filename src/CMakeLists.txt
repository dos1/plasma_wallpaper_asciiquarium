plasma_install_package(pkg org.kde.plasma.asciiquarium wallpapers Wallpaper)

set(asciiquarium_plugin_SRCS
    asciiquarium_plugin.cpp
)

add_library(org.kde.plasma.asciiquarium MODULE)
kcoreaddons_desktop_to_json(org.kde.plasma.asciiquarium pkg/metadata.desktop)

target_link_libraries(org.kde.plasma.asciiquarium
    Qt5::Gui
    Qt5::Quick
    KF5::Plasma
    )

install(TARGETS org.kde.plasma.asciiquarium DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/applets)
