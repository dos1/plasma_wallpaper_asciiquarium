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

Once installed, you may have to restart Plasma, for Plasma to recognize the
added wallpaper package and its accompanying C++ plugin.  The easiest way is to
logout of your desktop and log back in.

But you could also run `kquitapp5 plasmashell` from a Konsole and then run
`plasmashell` from Kicker (Alt-F2) or Konsole.

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

- Why?!?
    - Why not?
