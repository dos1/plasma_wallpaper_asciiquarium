import QtQuick 2.7
import QtQuick.Particles 2.0

/* Needed for the image provider */
import org.kde.plasma.asciiquarium 1.0

Image {
    source: "image://org.kde.plasma.asciiquarium/from_left/fish"
    cache: false

    id: shell

    function randBetween(l, r) {
        return Math.floor(l + (Math.random() * (r - l)));
    }

    property int moveStepX: 0
    property int msPerStep: randBetween(500, 1100)
    x: randBetween(40, 100) // TODO: Fix to be in step size increments
    y: randBetween(80, 400) // TODO: Fix to be in step size increments

    // Move in blocky steps
    AsciiquariumAnimator on x {
        // TODO: Make these properties aliases?
        moveStep: shell.moveStepX
        msPerStep: shell.msPerStep;
    }
}
