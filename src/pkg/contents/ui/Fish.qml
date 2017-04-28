import QtQuick 2.7
import QtQuick.Particles 2.0

/* Needed for the image provider */
import org.kde.plasma.asciiquarium 1.0

Image {
    id: shell

    function randBetween(l, r) {
        return Math.floor(l + (Math.random() * (r - l)));
    }

    property int moveStepX: 0
    property int msPerStep: randBetween(80, 1100)
    property bool leftFacing: false

    source: leftFacing ? "image://org.kde.plasma.asciiquarium/from_right/fish"
                       : "image://org.kde.plasma.asciiquarium/from_left/fish"
    cache: false

    // Move in blocky steps
    AsciiquariumAnimator on x {
        // TODO: Make these properties aliases?
        moveStep: shell.moveStepX
        msPerStep: shell.msPerStep
        leftFacing: shell.leftFacing
    }
}
