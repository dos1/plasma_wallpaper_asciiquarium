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

/* Needed for the image provider */
import org.kde.plasma.asciiquarium 1.0

Item {
    Rectangle {
        color: 'black'
        anchors.fill: parent
    }
    Image {
        source: "image://org.kde.plasma.asciiquarium/from_left/fish"
        anchors.left: parent.left
        anchors.top: parent.top
    }
    Image {
        visible: false // Here only so we can get its size.  It is shown elsewhere
        id: sharkPicture
        source: "image://org.kde.plasma.asciiquarium/from_left/shark"
    }
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
        property real screenWidth: (parent.width / monoFontMetrics.maximumCharacterWidth).toFixed(1)
        property real screenHeight: (parent.height / monoFontMetrics.lineSpacing).toFixed(1)
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
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: parent.width / 3

            emitRate: 0.5
            maximumEmitted: 1

            size: sharkPicture.width

            velocity: PointDirection {
                x: 60
                xVariation: 15
                y: 0
            }

            lifeSpan: 30000
            group: "sharks"
        }
        Emitter {
            id: leftFishEmitter

            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: parent.width / 3

            emitRate: 0.5
            size: 40
            velocity: PointDirection {
                x: 50
                xVariation: 20
                y: 0
            }
            lifeSpan: 30000
            group: "fish_from_left"
        }
        Emitter {
            id: rightFishEmitter

            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: parent.width / 3

            emitRate: 0.5
            size: 40
            velocity: PointDirection {
                x: -50
                xVariation: 20
                y: 0
            }
            lifeSpan: 30000
            group: "fish_from_right"
        }
        CustomParticle {
            groups: [ "fish_from_left" ]
            property variant source: Image {
                id: leftFishImage
                cache: false
                source: "image://org.kde.plasma.asciiquarium/from_left/fish"
            }
            property variant tex_w: leftFishImage.width
            property variant tex_h: leftFishImage.height
            vertexShader:"
                uniform float tex_w;
                uniform float tex_h;
                void main() {
                    highp float t = (qt_Timestamp - qt_ParticleData.x) / qt_ParticleData.y;

                    /* Needed to account for non-square size */
                    highp vec2 size = vec2(tex_w, tex_h);

                    highp vec2 pos = qt_ParticlePos
                                - (size / 2.)
                                + size * qt_ParticleTex
                                + qt_ParticleVec.xy * (t * qt_ParticleData.y);

                    pos.x = pos.x / 14.;
                    pos.y = pos.y / 28.;
                    pos = floor(pos);
                    pos.x = pos.x * 14.;
                    pos.y = pos.y * 28.;

                    qt_TexCoord0 = qt_ParticleTex;
                    gl_Position = qt_Matrix * vec4(pos.x, pos.y, 0, 1);
                }
            "
        }
        ItemParticle {
            groups: [ "fish_from_right" ]
            delegate: Component {
                Image {
                    cache: false
                    source: "image://org.kde.plasma.asciiquarium/from_right/fish"
                }
            }
        }
        CustomParticle {
            groups: [ "sharks" ]

            property variant source: sharkPicture // defined far above
            property variant tex_w: sharkPicture.width
            property variant tex_h: sharkPicture.height

            vertexShader:"
                uniform float tex_w;
                uniform float tex_h;
                void main() {
                    highp float t = (qt_Timestamp - qt_ParticleData.x) / qt_ParticleData.y;

                    /* Needed to account for non-square size */
                    highp vec2 size = vec2(tex_w, tex_h);

                    highp vec2 pos = qt_ParticlePos
                                - (size / 2.)
                                + size * qt_ParticleTex
                                + qt_ParticleVec.xy * (t * qt_ParticleData.y);

                    pos.x = pos.x / 14.;
                    pos.y = pos.y / 28.;
                    pos = floor(pos);
                    pos.x = pos.x * 14.;
                    pos.y = pos.y * 28.;

                    qt_TexCoord0 = qt_ParticleTex;
                    gl_Position = qt_Matrix * vec4(pos.x, pos.y, 0, 1);
                }
            "
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
