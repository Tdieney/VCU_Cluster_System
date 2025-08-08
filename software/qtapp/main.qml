import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtGraphicalEffects 1.15

ApplicationWindow {
    id: window
    visible: true
    width: 1024; height: 600
    minimumHeight: 600; maximumHeight: 600
    minimumWidth: 1024; maximumWidth: 1024
//    visibility: "FullScreen"
    title: "Instrument Cluster"
    color: "white"

    Component.onCompleted: {
        console.log("Window size:", width, height)
        console.log("Screen size:", Screen.width, Screen.height)
    }

    property real minSpeed: 0
    property real maxSpeed: 180

    readonly property real minRot:  -108
    readonly property real maxRot:   108

    property int speed: 0
    property string gear: "D"
    property int battery: 37
    property string time: {
        var now = new Date()
        now.setHours(now.getHours() + 7)  // For VietNam
        return Qt.formatTime(now, "hh:mm")
    }
    property string date: {
        var now = new Date()
        now.setHours(now.getHours() + 7)  // For VietNam
        return Qt.formatDate(now, "dd/MM/yyyy")
    }
    property string temp: "28°C"
    property int odometer: 38923
    property int trip: 443

    Timer {
        interval: 100; running: true; repeat: true
        onTriggered: {
            speed = (speed + 1) % 181
            battery = Math.max(0, battery + 1) % 101
            time = Qt.formatTime(new Date(), "hh:mm")
            date = Qt.formatDate(new Date(), "dd/MM/yyyy")
        }
    }

    FontLoader {
        id: myFont
        source: "qrc:/fonts/Aldrich-Regular.ttf"
    }

    Image {
        id: backgroundImage
        source: "qrc:/images/background.png"
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        z: 0
    }

    Image {
        id: centreArrowImage
        source: "qrc:/images/centre.png"
        x: (window.width - centreArrowImage.width) / 2
        y: (window.height - centreArrowImage.height) / 2 + 6
        fillMode: Image.PreserveAspectFit
        z: 2
    }

    Image {
        id: arrowImage
        source: "qrc:/images/arrow.png"
        x: (window.width - arrowImage.width) / 2
        y: (window.height - arrowImage.height) / 2 - 1
        rotation : speedToRotation(speed)
        z: 1
    }

    Rectangle {
        id: highBeamRec
        width: 40
        height: 40
        x: 293
        y: 6
        color: "transparent"

        Image {
            id: highBeamImage
            source: "qrc:/images/high_beam.png"
            visible: false
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
        }
    }

    Rectangle {
        id: lowBeamRec
        width: 40
        height: 40
        x: 376
        y: 9
        color: "transparent"

        Image {
            id: lowBeamImage
            source: "qrc:/images/low_beam.png"
            visible: false
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
        }
    }

    Rectangle {
        id: parkingLightsRec
        width: 40
        height: 40
        x: 612
        y: 7
        color: "transparent"

        Image {
            id: parkingLightsImage
            source: "qrc:/images/parking_lights.png"
            visible: false
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
        }
    }

    Rectangle {
        id: hazardLightsRec
        width: 40
        height: 40
        x: 692
        y: 7
        color: "transparent"

        Image {
            id: hazardLightsImage
            source: "qrc:/images/hazard.png"
            visible: false
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
        }
    }

    Rectangle {
        id: turnLeftRec
        width: 40
        height: 40
        x: 39
        y: 77
        color: "transparent"

        Image {
            id: turnLeftImage
            source: "qrc:/images/left_arrow.png"
            visible: false
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
        }
    }

    Rectangle {
        id: turnRightRec
        width: 40
        height: 40
        x: 945
        y: 77
        color: "transparent"

        Image {
            id: turnRightImage
            source: "qrc:/images/right_arrow.png"
            visible: false
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
        }
    }

    Text {
        text: "Distance"
        x: 770
        y: 212
        color: "#EF4D7D"
        font.family: myFont.name
        font.pixelSize: 32
        horizontalAlignment: Text.AlignLeft
    }

    Text {
        text: "230 KM"
        x: 770
        y: 256
        color: "white"
        font.family: myFont.name
        font.pixelSize: 32
        horizontalAlignment: Text.AlignLeft
    }

    Text {
        text: "Temperature"
        x: 770
        y: 312
        color: "#EF4D7D"
        font.family: myFont.name
        font.pixelSize: 32
        horizontalAlignment: Text.AlignLeft
    }

    Text {
        text: temp
        x: 770
        y: 356
        color: "white"
        font.family: myFont.name
        font.pixelSize: 32
        horizontalAlignment: Text.AlignLeft
    }

    Rectangle {
        id: batRec
        width: 64 * (battery / 100)
        height: 24
        x: 773
        y: 557
        radius: 3

        color: {
            if (battery > 50) return "#36c75b"
            else if (battery > 20) return "#fbca0a"
            else return "#f70e02"
        }

        Behavior on color {
            ColorAnimation {
                duration: 500
            }
        }
    }

    Rectangle {
        id: batTextRec
        width: 100
        height: 45
        x: 850
        y: 556
        color: "transparent"

        Text {
            id: batText
            text: battery + "%"
            color: "white"
            font.family: myFont.name
            font.pixelSize: 32
            anchors.right: parent.right
            horizontalAlignment: Text.AlignLeft
        }
    }

    Text {
        id: timeText
        text: time
        x: (window.width - timeText.width) / 2
        y: 15
        color: "white"
        font.family: myFont.name
        font.pixelSize: 32
        horizontalAlignment: Text.AlignHCenter
    }

    Text {
        id: dateText
        text: date
        x: 780
        y: 21
        color: "white"
        font.family: myFont.name
        font.pixelSize: 32
        horizontalAlignment: Text.AlignHCenter
    }

    Text {
        text: "ODO:"
        x: 37
        y: 21
        color: "white"
        font.family: myFont.name
        font.pixelSize: 32
        horizontalAlignment: Text.AlignHCenter
    }

    Rectangle {
        id: odometerRec
        width: 115
        height: 50
        x: 128
        y: 21
        color: "transparent"

        Text {
            id: odometerText
            text: odometer
            color: "white"
            font.family: myFont.name
            font.pixelSize: 32
            anchors.right: parent.right
            horizontalAlignment: Text.AlignRight
        }
    }

    Rectangle {
        id: gearRec
        width: 202
        height: 50
        x: 65
        y: 547
        color: "transparent"

        // Gear selector
        RowLayout {
            anchors.centerIn: parent
            spacing: 20

            Repeater {
                model: ["P", "R", "N", "D"]
                delegate: Text {
                    text: modelData
                    font.pixelSize: 32
                    font.family: myFont.name
                    color: gear === modelData ? "white" : "#AAAAAA"
                    horizontalAlignment: Text.AlignRight
                }
            }
        }
    }

    Connections {
        target: canHandler
        onLeftLightChanged: turnLeftImage.visible = leftLight
        onRightLightChanged: turnRightImage.visible = rightLight
        onHazardLightsChanged: hazardLightsImage.visible = hazardLights
        onHighBeamChanged: highBeamImage.visible = highBeam
        onLowBeamChanged: lowBeamImage.visible = lowBeam
        onParkingLightsChanged: parkingLightsImage.visible = parkingLights
    }

    function speedToRotation(speed) {
        var t = (speed - minSpeed) / (maxSpeed - minSpeed)
        return minRot + (maxRot - minRot) * t
    }
}
