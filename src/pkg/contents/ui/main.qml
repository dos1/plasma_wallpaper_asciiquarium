import QtQuick 2.7
import QtQuick.Particles 2.0

// Needed for the image provider and AsciiquariumAnimator
import org.kde.plasma.asciiquarium 1.0

Item {
    function randBetween(l, r) {
        return Math.floor(l + (Math.random() * (r - l)));
    }

    id: rootItem

    property var screenWidth : Math.floor(width  / asciiquariumCellWidth)
    property var screenHeight: Math.floor(height / asciiquariumCellHeight)

    Component.onCompleted: {
        var component = Qt.createComponent("Fish.qml")
        var parentWidth  = Math.floor(fishSystem.width  / asciiquariumCellWidth)
        var parentHeight = Math.floor(fishSystem.height / asciiquariumCellHeight)

        if (component.status == Component.Ready) {
            for (var i = 0; i < 100; i++) {
                var startX = randBetween(0, Math.floor(parentWidth / 3))
                startX = startX * asciiquariumCellWidth
                var leftFacing = (i % 2 < 1)

                if (leftFacing) {
                    startX = (parentWidth - 10) * asciiquariumCellWidth - startX
                }

                var startY = randBetween(0, parentHeight - 6)
                startY = startY * asciiquariumCellHeight

                // x, y are in terms of fishSystem, not rootItem

                try {
                    var fishy = component.createObject(fishSystem, {
                        "leftFacing": leftFacing,
                        "moveStepX": Qt.binding(function() {
                            return asciiquariumCellWidth
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
    BorderImage {
        y: 5 * asciiquariumCellHeight
        anchors.left: parent.left
        anchors.right: parent.right
        id: oceanTop
        source: "image://org.kde.plasma.asciiquarium/ocean"
        cache: false
        smooth: false
        horizontalTileMode: BorderImage.Repeat
    }
    Image {
        id: castle
        x: asciiquariumCellWidth  * (rootItem.screenWidth  - 32)
        y: asciiquariumCellHeight * (rootItem.screenHeight - 13)
        source: "image://org.kde.plasma.asciiquarium/castle"
        z: 1
    }
    Image {
        visible: false // Here only so we can get its size.  It is shown elsewhere
        id: sharkPicture
        source: "image://org.kde.plasma.asciiquarium/from_left/shark"
    }
    ParticleSystem {
        id: fishSystem
        anchors.top: oceanTop.bottom
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 7 * asciiquariumCellHeight // Keep fish out of the seaweed
        anchors.left: parent.left
        anchors.right: parent.right

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
            property double cell_w: asciiquariumCellWidth
            property double cell_h: asciiquariumCellHeight

            vertexShader:"
                uniform float tex_w;
                uniform float tex_h;
                uniform float cell_w;
                uniform float cell_h;

                void main() {
                    highp float t = (qt_Timestamp - qt_ParticleData.x) / qt_ParticleData.y;

                    /* Needed to account for non-square size */
                    highp vec2 size = vec2(tex_w, tex_h);

                    highp vec2 pos = qt_ParticlePos
                                - (size / 2.)
                                + size * qt_ParticleTex
                                + qt_ParticleVec.xy * (t * qt_ParticleData.y);

                    pos.x = pos.x / cell_w;
                    pos.y = pos.y / cell_h;
                    pos = floor(pos);
                    pos.x = pos.x * cell_w;
                    pos.y = pos.y * cell_h;

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
