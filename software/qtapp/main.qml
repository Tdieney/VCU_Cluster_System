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
    visibility: "FullScreen"
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
    property real delta_distance_km: 0
    property real trip: 0
    property real blendingFactor: 0
    property real avg_consumption_short_term: 0
    property real final_avg_consumption: 0
    property real dte_km: 0

    property int battery: 0
    property real instant_consumption_kWh_per_100km: 0
    property real energy_used_kWh_this_tick: 0
    property real cumulative_energy_used_kWh: 0
    property real energy_remaining_kWh: 0
    readonly property int battery_capacity_kWh: 75
    readonly property int initial_SoC_percent: 100
    
    property var analogBuffer: []      // Buffer for recent analog values
    property int analogBufferSize: 10  // Number of samples for smoothing

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
    property real odometer: 38923

    Timer {
        interval: 100; running: true; repeat: true
        onTriggered: {
            time = Qt.formatTime(new Date(), "hh:mm")
            date = Qt.formatDate(new Date(), "dd/MM/yyyy")

            delta_distance_km = speed * (0.1 / 3600)

            trip += delta_distance_km
            odometer += delta_distance_km

            // Calculate DTE (Distance to Empty)
            blendingFactor = Math.min(trip / 50.0, 1.0)
            if (trip > 0.0) {
                avg_consumption_short_term =
                    0.9 * avg_consumption_short_term +
                    0.1 * consumptionFromSpeed(speed)
            }
            // Calculate avg_consumption_long_term
            var avg_consumption_long_term = 0.0
            if (trip > 0.0) {
                avg_consumption_long_term = (cumulative_energy_used_kWh / trip) * 100
            }
            final_avg_consumption = Math.max((blendingFactor * avg_consumption_short_term) +
                        ((1.0 - blendingFactor) * avg_consumption_long_term), 0.5); // kWh/100km
            dte_km = (energy_remaining_kWh / final_avg_consumption) * 100.0;

            // Calculate battery
            instant_consumption_kWh_per_100km = consumptionFromSpeed(speed)
            energy_used_kWh_this_tick = instant_consumption_kWh_per_100km * delta_distance_km / 100.0

            cumulative_energy_used_kWh += energy_used_kWh_this_tick
            energy_remaining_kWh = battery_capacity_kWh * (initial_SoC_percent / 100.0) - cumulative_energy_used_kWh

            battery = Math.max(0, Math.min(100, (energy_remaining_kWh / battery_capacity_kWh) * 100))
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
        id: tripText
        text: "Trip"
        x: 775
        y: 190
        color: "#EF4D7D"
        font.family: myFont.name
        font.pixelSize: 28
        horizontalAlignment: Text.AlignLeft
    }

    Text {
        text: trip.toFixed(1) + " KM"
        x: 775
        y: tripText.y + 38
        color: "white"
        font.family: myFont.name
        font.pixelSize: 28
        horizontalAlignment: Text.AlignLeft
    }

    Text {
        id: distanceText
        text: "Distance"
        x: 775
        y: 270
        color: "#EF4D7D"
        font.family: myFont.name
        font.pixelSize: 28
        horizontalAlignment: Text.AlignLeft
    }

    Text {
        text: dte_km.toFixed(1) + " KM"
        x: 775
        y: distanceText.y + 38
        color: "white"
        font.family: myFont.name
        font.pixelSize: 28
        horizontalAlignment: Text.AlignLeft
    }

    Text {
        id: tempText
        text: "Temperature"
        x: 775
        y: 350
        color: "#EF4D7D"
        font.family: myFont.name
        font.pixelSize: 28
        horizontalAlignment: Text.AlignLeft
    }

    Text {
        text: temp
        x: 775
        y: tempText.y + 38
        color: "white"
        font.family: myFont.name
        font.pixelSize: 28
        horizontalAlignment: Text.AlignLeft
    }

    Rectangle {
        id: speedRec
        width: 128
        height: 100
        x: 100
        y: 424
        color: "transparent"

        Column {
            anchors.centerIn: parent
            spacing: 4

            Text {
                id: speedText
                text: speed
                color: "white"
                font.family: myFont.name
                font.pixelSize: 64
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: "KM/H"
                color: "white"
                font.family: myFont.name
                font.pixelSize: 24
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
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
            text: odometer.toFixed(0)
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
        onSpeedChanged: {
            // Add new value to buffer
            analogBuffer.push(analogVal)
            if (analogBuffer.length > analogBufferSize)
                analogBuffer.shift() // Remove oldest

            // Calculate average
            var sum = 0
            for (var i = 0; i < analogBuffer.length; ++i)
                sum += analogBuffer[i]
            var avgAnalog = sum / analogBuffer.length

            // Use average for speed calculation
            speed = avgAnalog * maxSpeed / 4000;
            speed = Math.max(minSpeed, Math.min(maxSpeed, speed));
        }
    }

    function speedToRotation(speed) {
        var t = (speed - minSpeed) / (maxSpeed - minSpeed)
        return minRot + (maxRot - minRot) * t
    }

    function consumptionFromSpeed(v) {
        // Simulation coefficients (can be adjusted for your vehicle)
        let a = 0.0006;  // air resistance coefficient
        let b = 0.01;   // rolling resistance and drivetrain loss
        let c = 10.0;  // fixed auxiliary load (kWh/100km)

        return a * v * v + b * v + c;  // kWh/100km
    }

}
