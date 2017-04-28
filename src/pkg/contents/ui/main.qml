import QtQuick 2.7
import QtQuick.Particles 2.0

// Needed for the image provider and AsciiquariumAnimator
import org.kde.plasma.asciiquarium 1.0

Item {
    function randBetween(l, r) {
        return Math.floor(l + (Math.random() * (r - l)));
    }

    id: rootItem

    Component.onCompleted: {
        var component = Qt.createComponent("Fish.qml")

        if (component.status == Component.Ready) {
            for (var i = 0; i < 100; i++) {
                var startX = randBetween(0, Math.floor(stats.screenWidth / 3))
                startX = startX * stats.moveStepX
                var leftFacing = (i % 2 < 1)

                if (leftFacing) {
                    startX = rootItem.width - stats.moveStepX * 10 - startX
                }

                var startY = randBetween(8, Math.floor(stats.screenHeight) - 10)
                startY = startY * stats.moveStepY

                try {
                    var fishy = component.createObject(fishSystem, {
                        "leftFacing": leftFacing,
                        "moveStepX": Qt.binding(function() {
                            return stats.moveStepX
                        }),
                        "x": startX,
                        "y": startY,
                    })
                }
                catch(err) {
                    for (var i = 0; i < err.qmlErrors.length; i++) {
                        console.log("Couldn't create fish " + error.qmlErrors[i].message)
                    }
                }
            }
        }
        else if (component.status == Component.Error) {
            console.log("Component failed to load, error ", component.errorString())
        }
    }

    Rectangle {
        color: 'black'
        anchors.fill: parent
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
        id: stats
        color: "green"

        property int moveStepX: monoFontMetrics.maximumCharacterWidth
        property int moveStepY: monoFontMetrics.lineSpacing
        property real screenWidth: (parent.width / moveStepX).toFixed(1)
        property real screenHeight: (parent.height / moveStepY).toFixed(1)

        text: "Characters are " + moveStepX + " by " + moveStepY + " wide. (" + screenWidth + ", " + screenHeight + ")"
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
