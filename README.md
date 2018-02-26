# Introduction

Plasma Asciiquarium -- It's a little ASCII-style aquarium, whose visual style
is adopted from a TTY-based program <q>Asciiquarium</q> written by Kirk Baucom.
You can find the original at
http://www.robobunny.com/projects/asciiquarium/html/

Back in 2005 Maksim Orlovich and I ported this to a KDE 3 screensaver.  I later
ported it to KDE's Plasma 4 (using the old KDE 3-style screensaver libraries),
but never quite got around to Plasma 5.

Well, the time has finally come... and now instead of being only a screensaver,
Asciiquarium is now a full-fledged Plasma Wallpaper, ported to using QML.

# Dependencies

Like other Plasma 5 software, this plugin depends on Qt 5 and parts of the KDE Frameworks 5.

Qt 5:

- Qt5Quick
  - Qt5Quick.Particles (sometimes this is packaged separately. like
    qtdeclarative5-particles-plugin on Ubuntu)
- Qt5Gui
- Qt5Qml

Normally these can be installed by installing devel packages like
`qt5declarative-dev` and `qt5base-dev`, and should also be installed as
prerequisites for the KF5 packages.  However the Particles plugin is not a
required dependency of any Plasma package so you may have to find that one
yourself.

KDE Frameworks 5:

- KF5::Config
- KF5::ConfigWidgets
- KF5::CoreAddons
- KF5::Declarative
- KF5::I18n
- KF5::IconThemes
- KF5::KIO
- KF5::Plasma
- KF5::Package

Normally these can all be met with devel packages for KF5::Plasma, KF5::KIO,
and KF5::Declarative (e.g. libkf5declarative-dev, libkf5plasma-dev, and
libkf5kio-dev on Debian-alikes).

# Building

It builds like any other CMake-based application targeting KDE Frameworks and
Plasma 5:

    tar xf plasma_wallpaper_asciiquarium-0.1.tar.xz
    cd plasma_wallpaper_asciiquarium-0.1
    mkdir build
    cd build
    cmake ../  #  Add -DCMAKE_INSTALL_PREFIX=/path/to/install if necessary
    make -j8 && make install

# Enabling

Wherever you install the plugin to, Plasma has to be able to find it.  So if you're setting up
a custom install location (whether that's /usr/local or under your home directory), you may need
to setup your environment variables properly for Plasma (and Qt) to find it.

In particular, I believe you need at least (where `$PREFIX` is the path you
passed to CMake as the install prefix):

    XDG_DATA_DIRS="$PREFIX/share:$XDG_DATA_DIRS"
    QML2_IMPORT_PATH="$PREFIX/lib/qml"

See the [Qt QML Import docs](http://doc.qt.io/qt-5/qtqml-syntax-imports.html)
for more details on how it loads QML modules.

I've heard of systems that have the `lib` dir in even crazier places, like
`/usr/local/lib/x86_64-pc-linux-gnu/qml`.  Wherever those QML libraries get
installed, Qt needs to know about it.

Once installed, you may have to restart Plasma, for Plasma to recognize the
added wallpaper package and its accompanying C++ plugin.  The easiest way is to
logout of your desktop and log back in.

But you could also run `kquitapp5 plasmashell` from a Konsole, reset the
environment as described above, and then run `plasmashell` from Kicker (Alt-F2)
or Konsole.

However you restart Plasma, simply right-click on your desktop and select
"Configure Desktop...".  In the "Wallpaper" tab you should be able to select a
new Wallpaper Type, "Plasma Asciiquarium".  Do so and click "OK" and you should
see glorious animated fishies swimming around on your desktop.

## Adjusting the Screen Locker

You can also use the Plasma Asciiquarium as a wallpaper for the screen locker.
Configure the screen locker (either in System Settings -> Desktop Behavior or
by running `kcmshell5 screenlocker`).  From there you can select the "Wallpaper"
tab and adjust the Wallpaper Type as you did for your desktop.

# FAQ

- How is the performance?
    - It's implemented mostly in QML (with a C++ shim to turn the text-based
      sprites into pixmaps that can be drawn on screen, and to implement the
      TTY-style animations).  On modern systems this should actually be quite
      efficient but if you are using a software GL renderer like `llvmpipe` or
      the QML software renderer available in newer Qt then you will likely see
      increased CPU usage.

- Where's the bubbles and stuff?
    - Yeah... I still need to implement the less straightforward animations. :(

- How much CPU is this thing going to use?
    - On my system on those of some others who have used it, it adds about 10%
      CPU usage between Plasma and KWin (which has to composite the result).

- Why?!?
    - Why not?
