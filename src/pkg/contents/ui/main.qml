/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.7
import QtQuick.Particles 2.0

Item {
    Rectangle {
        color: "black"
        anchors.fill: parent
    }
/*    Image {
        source: "image://org.kde.plasma.asciiquarium/black"
        anchors.fill: parent
    }
    */
    Text {
        font.family: "monospace"
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        id: "label"
        text: "Hello Asciiquarium World!"
        color: "white"
    }
    FontMetrics {
        id: monoFontMetrics
        font.family: "monospace"
    }
    Text {
        anchors.top: label.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        id: "stats"
        color: "green"
        property real screenWidth: parent.width / monoFontMetrics.maximumCharacterWidth
        property real screenHeight: parent.height / monoFontMetrics.lineSpacing
        text: "Characters are " + monoFontMetrics.maximumCharacterWidth + " by " + monoFontMetrics.lineSpacing + " wide. (" + screenWidth + ", " + screenHeight + ")"
    }
    ParticleSystem {
        id: fishSystem
        anchors.fill: parent

        /*
        Emitter {
            id: castleEmitter
            anchors.bottom: parent.bottom
            height: 200
        }
        */
        Emitter {
            id: sharkEmitter
            anchors.fill: parent
            emitRate: 0.5
            maximumEmitted: 1
            size: 80
            velocity: PointDirection {
                x: 60
                xVariation: 15
                y: 0
            }
            lifeSpan: 30000
            group: "sharks"
        }
        Emitter {
            id: fishEmitter
            anchors.fill: parent
            emitRate: 0.5
            size: 40
            velocity: PointDirection {
                x: 50
                xVariation: 20
                y: 0
            }
            lifeSpan: 30000
            group: "fish"
            onEmitParticles: {
                for (var i = 0; i < particles.length; i++) {
                    var particle = particles[i];
                    // Make some go left instead of right
                    particle.initialVX = ((Math.random() < 0.5) ? 1 : -1) * particle.initialVX;
                }
            }
        }
        ItemParticle {
            groups: [ "fish" ]
            delegate: Component {
                Rectangle {
                    id: fishDelegate
                    height: 60
                    width: 120
                    color: "green"
                }
            }
        }
        ItemParticle {
            groups: [ "sharks" ]
            delegate: Component {
                Rectangle {
                    id: sharkDelegate
                    height: 200
                    width: 400
                    color: "red"
                }
            }
        }
        Age {
            /* Kills things at left side of screen */
            id: leftSpriteRemover
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 1
            lifeLeft: 0
            advancePosition: false
        }
        Age {
            /* Kills things at right side of screen */
            id: rightSpriteRemover
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 1
            lifeLeft: 0
            advancePosition: false
        }
    }
}
