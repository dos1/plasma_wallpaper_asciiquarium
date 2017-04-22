plasma_install_package(pkg org.kde.plasma.asciiquarium wallpapers wallpaper)

set(asciiquarium_plugin_SRCS
    asciiquarium_plugin.cpp
)

add_library(plasma_wallpaper_asciiquarium SHARED
    ${asciiquarium_plugin_SRCS}
    )
kcoreaddons_desktop_to_json(plasma_wallpaper_asciiquarium pkg/metadata.desktop)

target_link_libraries(plasma_wallpaper_asciiquarium
    Qt5::Gui
    Qt5::Quick
    KF5::Plasma
    )

set_target_properties(plasma_wallpaper_asciiquarium PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

install(TARGETS plasma_wallpaper_asciiquarium DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/asciiquarium)
install(FILES   qmldir                        DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/asciiquarium)
install(FILES   pkg/contents/ui/Fish.qml      DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/asciiquarium)
