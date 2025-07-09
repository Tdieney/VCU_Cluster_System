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

    // Dynamic scaling properties based on window size
    readonly property real baseWidth: 1024
    readonly property real baseHeight: 600
    readonly property real scaleFactor: Math.min(width / baseWidth, height / baseHeight)
    readonly property real spacing: Math.max(4, width * 0.005)
    readonly property real margin: Math.max(8, width * 0.005)

    // Font size scaling
    readonly property real largeFontSize: Math.max(72, width * 0.094)
    readonly property real mediumFontSize: Math.max(18, width * 0.02)
    readonly property real smallFontSize: Math.max(16, width * 0.018)
    readonly property real gearFontSize: Math.max(20, width * 0.023)

    Component.onCompleted: {
        console.log("Window size:", width, height)
        console.log("Screen size:", Screen.width, Screen.height)
        console.log("Scale factor:", scaleFactor)
    }

    property int speed: 0
    property string gear: "D"
    property int battery: 0
    property string time: {
        var now = new Date()
        now.setHours(now.getHours() + 7)  // For VietNam
        return Qt.formatTime(now, "hh:mm")
    }
    property string temp: "28°C"
    property int odometer: 3813
    property int trip: 443

    Timer {
        interval: 100; running: true; repeat: true
        onTriggered: {
            speed = (speed + 1) % 300
            battery = Math.max(0, battery + 1) % 100
            time = Qt.formatTime(new Date(), "hh:mm")
        }
    }

    GridLayout {
        id: mainLayout
        anchors.fill: parent
        columns: 4
        rowSpacing: spacing
        columnSpacing: spacing
        anchors.margins: margin

        // Warning lights
        Rectangle {
            id: indicatorLightRec
            Layout.column: 0
            Layout.row: 0
            Layout.alignment: Qt.AlignTop
            Layout.preferredWidth: parent.width * 0.1
            Layout.preferredHeight: parent.height * 0.8
            Layout.maximumWidth: parent.width * 0.35
            Layout.maximumHeight: parent.height * 0.8
            color: "black"

            ColumnLayout {
                id: indicatorLightCL

                anchors.fill: parent
                anchors.topMargin: 0
                anchors.bottomMargin: 50
                spacing: 5


                Image {
                    id: img_1
                    source: "qrc:/images/low_beam_indicator.png"
                    fillMode: Image.PreserveAspectFit
                    Layout.alignment: Qt.AlignLeft
                    Layout.preferredWidth: 40
                    Layout.preferredHeight: 40
                }

                Image {
                    id: img_2
                    source: "qrc:/images/high_beam_indicator.png"
                    fillMode: Image.PreserveAspectFit
                    Layout.alignment: Qt.AlignLeft
                    Layout.preferredWidth: 40
                    Layout.preferredHeight: 40
                }

                Image {
                    id: img_3
                    source: "qrc:/images/front_fog.png"
                    fillMode: Image.PreserveAspectFit
                    Layout.alignment: Qt.AlignLeft
                    Layout.preferredWidth: 40
                    Layout.preferredHeight: 40
                }

                Image {
                    id: img_4
                    source: "qrc:/images/rear_fog.png"
                    fillMode: Image.PreserveAspectFit
                    Layout.alignment: Qt.AlignLeft
                    Layout.preferredWidth: 40
                    Layout.preferredHeight: 40
                }

                Image {
                    id: img_5
                    source: "qrc:/images/seatbelt.png"
                    fillMode: Image.PreserveAspectFit
                    Layout.alignment: Qt.AlignLeft
                    Layout.preferredWidth: 40
                    Layout.preferredHeight: 40
                }
            }
        }

        // Car image - takes 40% of width
//        Image {
//            id: carImage
//            source: "qrc:/images/sport_car.png"
//            Layout.column: 1
//            Layout.row: 0
//            Layout.rowSpan: 2
//            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
//            Layout.preferredWidth: parent.width * 0.35
//            Layout.preferredHeight: parent.height * 0.9
//            Layout.maximumWidth: parent.width * 0.35
//            Layout.maximumHeight: parent.height * 0.9
//            fillMode: Image.PreserveAspectFit
//            smooth: true
//        }
        Rectangle {
            id: carImage
            Layout.column: 1
            Layout.row: 0
            Layout.alignment: Qt.AlignCenter
            Layout.preferredWidth: parent.width * 0.35
            Layout.preferredHeight: parent.height * 0.4
            Layout.maximumWidth: parent.width * 0.35
            Layout.maximumHeight: parent.height * 0.4
            color: "black"
        }

        // Speed display - takes 35% of width
        Rectangle {
            id: speedRec
            Layout.column: 2
            Layout.row: 0
            Layout.alignment: Qt.AlignCenter
            Layout.preferredWidth: parent.width * 0.35
            Layout.preferredHeight: parent.height * 0.9
            Layout.maximumWidth: parent.width * 0.35
            Layout.maximumHeight: parent.height * 0.9
            color: "black"

            ColumnLayout {
                anchors.centerIn: parent
                spacing: parent.height * 0.02

                Text {
                    text: speed
                    font.pixelSize: largeFontSize
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter
                    color: "black"
                }

                Text {
                    text: "KM/H"
                    font.pixelSize: mediumFontSize
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter
                    color: "black"
                }

                Item {
                    Layout.preferredHeight: speedRec.height * 0.02
                }

                Rectangle {
                    id: barBg
                    Layout.preferredWidth: speedRec.width * 0.75
                    Layout.preferredHeight: Math.max(4, speedRec.height * 0.015)
                    radius: height * 0.3
                    color: "#AAAAAA"
                    Layout.alignment: Qt.AlignHCenter

                    Rectangle {
                        width: parent.width * Math.min(speed / 300, 1)
                        height: parent.height
                        radius: parent.radius
                        color: "black"

                        Behavior on width {
                            NumberAnimation {
                                duration: 100
                                easing.type: Easing.OutQuad
                            }
                        }
                    }
                }
            }
        }

        // Gear selector - takes remaining 25% of width
        Rectangle {
            Layout.column: 3
            Layout.row: 0
            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            Layout.preferredWidth: parent.width * 0.03
            height: parent.height * 0.9
            width: parent.width * 0.03
            color: "black"

            Repeater {
                model: ["P", "R", "N", "D"]
                delegate: Text {
                    text: modelData
                    font.pixelSize: gearFontSize
                    color: gear === modelData ? "black" : "lightgray"
                    horizontalAlignment: Text.AlignRight
                    width: parent.width
                    anchors.right: parent.right

                    // Even vertical spacing based on index
                    y: index * (parent.height * 0.02 + font.pixelSize) + parent.height * 0.35

                    Behavior on color {
                        ColorAnimation {
                            duration: 200
                        }
                    }
                }
            }
        }

        // Bottom Info Bar - spans full width
        RowLayout {
            Layout.column: 0
            Layout.columnSpan: 3
            Layout.row: 2
            Layout.alignment: Qt.AlignBottom
            Layout.fillWidth: true
            spacing: parent.width * 0.04

            Text {
                text: time
                font.pixelSize: mediumFontSize
                color: "black"
            }

            Text {
                text: temp
                font.pixelSize: mediumFontSize
                color: "black"
            }

            Item { Layout.fillWidth: true } // Flexible spacer

            Text {
                text: trip + "KM / " + odometer + "KM"
                font.pixelSize: mediumFontSize
                color: "black"
                horizontalAlignment: Text.AlignHCenter
            }

            Item { Layout.fillWidth: true } // Flexible spacer

            RowLayout {
                spacing: parent.width * 0.008
                Layout.alignment: Qt.AlignRight

                Rectangle {
                    id: batteryBar
                    Layout.preferredWidth: Math.max(80, parent.parent.width * 0.1)
                    Layout.preferredHeight: Math.max(16, parent.parent.height * 0.03)
                    radius: height * 0.15
                    color: "lightgray"

                    Rectangle {
                        width: parent.width * (battery / 100)
                        height: parent.height
                        radius: parent.radius
                        color: {
                            if (battery > 50) return "green"
                            else if (battery > 20) return "orange"
                            else return "red"
                        }

                        Behavior on width {
                            NumberAnimation {
                                duration: 200
                                easing.type: Easing.OutQuad
                            }
                        }

                        Behavior on color {
                            ColorAnimation {
                                duration: 300
                            }
                        }
                    }
                }

                Text {
                    text: battery + "%"
                    font.pixelSize: smallFontSize
                    color: "black"
                    Layout.preferredWidth: Math.max(35, parent.parent.width * 0.04)
                    horizontalAlignment: Text.AlignLeft
                }
            }
        }
    }
}
