#ifndef ASCIIQUARIUM_WALLPAPER_PLUGIN_H
#define ASCIIQUARIUM_WALLPAPER_PLUGIN_H

/*
 * Asciiquarium - Native KDE Screensaver based on the Asciiquarium program
 *    (c) Kirk Baucom <kbaucom@schizoid.com>, which you can find at
 *    http://www.robobunny.com/projects/asciiquarium/
 *
 * Ported to KDE-based desktops years ago by Maksim Orlovich <maksim@kde.org>
 * and Michael Pyne <mpyne@kde.org>.
 *
 * Copyright (c) 2003 Kirk Baucom     <kbaucom@schizoid.com>
 * Copyright (c) 2005, 2008, 2012, 2017 Michael Pyne <mpyne@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */
#include <QQmlExtensionPlugin>
#include <QQmlPropertyValueSource>
#include <QQmlProperty>
#include <QTimer>

class AsciiquariumPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void initializeEngine(QQmlEngine *engine, const char *uri) override;
    void registerTypes(const char *uri) override;
};

/**
 * Used to translate our animated fish and other sprites in staggered steps
 * instead of smoothly, to help better simulate the old TTY feel.  In other
 * words instead of moving one pixel at a time (or whatever would be
 * appropriate at the monitor refresh rate), we move by the number of pixels in
 * one character at a time.
 */
class AsciiquariumAnimator : public QObject, public QQmlPropertyValueSource
{
    Q_OBJECT
    Q_INTERFACES(QQmlPropertyValueSource)
    Q_PROPERTY(int moveStep READ moveStep WRITE setMoveStep)
    Q_PROPERTY(int msPerStep READ msPerStep WRITE setMsPerStep)
    Q_PROPERTY(bool leftFacing READ leftFacing WRITE setLeftFacing)

public:
    AsciiquariumAnimator(QObject *parent = 0);

    int moveStep() const { return m_moveStep; }
    void setMoveStep(int newMoveStep) { m_moveStep = newMoveStep; }

    int msPerStep() const { return m_msPerStep; }
    void setMsPerStep(int newMsPerStep);

    bool leftFacing() const { return m_leftFacing; }
    void setLeftFacing(bool newLeftFacing) { m_leftFacing = newLeftFacing; }

    virtual void setTarget(const QQmlProperty &property) override
    {
        m_targetProperty = property;
    }

private Q_SLOTS:
    void updateProperty();

private:
    QQmlProperty m_targetProperty;
    QTimer m_timer;
    int m_moveStep = 8;
    int m_msPerStep = 0;
    bool m_leftFacing = false;
};

#endif /* ASCIIQUARIUM_WALLPAPER_PLUGIN_H */
