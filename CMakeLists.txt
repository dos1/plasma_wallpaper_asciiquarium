cmake_minimum_required(VERSION 3.0)

project(PlasmaWallpaperAsciiquarium VERSION "0.1")
set(KF5_DEP_VERSION "5.32.0")
set(REQUIRED_QT_VERSION 5.6.0)

# ECM setup
include(FeatureSummary)
find_package(ECM 5.32.0 NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

# Used by KDECMakeSettings
set(KDE_SKIP_TEST_SETTINGS FALSE)

include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(KDEInstallDirs)
include(KDECMakeSettings NO_POLICY_SCOPE)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(CMakePackageConfigHelpers)
include(ECMSetupVersion)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMQtDeclareLoggingCategory)
include(KDEPackageAppTemplates)

ecm_setup_version(PROJECT
                  VARIABLE_PREFIX PW_ASCIIQUARIUM
                  SOVERSION 0
                  )

set(KDE_NO_DEPRECATED TRUE)
set(CMAKE_AUTOMOC_MOC_OPTIONS "-DKDE_NO_DEPRECATED")

if(UNIX)
   add_definitions("-Wall -W -Wextra -std=c++11")
endif()

find_package(Qt5 ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE COMPONENTS Quick Gui Qml)

find_package(KF5Plasma ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5Config ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5ConfigWidgets ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5CoreAddons ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5Declarative ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5I18n ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5IconThemes ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5KIO ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5Package ${KF5_DEP_VERSION} REQUIRED)

#########################################################################

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)
add_definitions(-DQT_USE_QSTRINGBUILDER)
add_definitions(-DQT_NO_URL_CAST_FROM_STRING)

remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_STRICT_ITERATORS -DQT_NO_CAST_FROM_BYTEARRAY -DQT_NO_KEYWORDS)

#########################################################################

add_definitions(-DTRANSLATION_DOMAIN=\"plasma5_wallpaper_asciiquarium\")
if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ki18n_install(po)
#    kdoctools_install(po)
endif()

# TODO: Get the C++ support to build, e.g. for QQuickImageProvider to supply
# pixmaps to the QML layout.
#add_subdirectory(pkg)
add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
