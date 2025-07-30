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
        id: gridIcon
        columns: 2
        rows: 3
        rowSpacing: 20
        columnSpacing: 20

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 10

        Image {
            source: "qrc:/images/high_beam.png"
            Layout.preferredWidth: 45
            Layout.preferredHeight: 45
            fillMode: Image.PreserveAspectFit
        }

        Image {
            source: "qrc:/images/low_beam.png"
            Layout.preferredWidth: 45
            Layout.preferredHeight: 45
            fillMode: Image.PreserveAspectFit
        }

        Image {
            source: "qrc:/images/front_fog.png"
            Layout.preferredWidth: 45
            Layout.preferredHeight: 45
            fillMode: Image.PreserveAspectFit
        }

        Image {
            source: "qrc:/images/seatbelt.png"
            Layout.preferredWidth: 45
            Layout.preferredHeight: 45
            fillMode: Image.PreserveAspectFit
        }

        Image {
            source: "qrc:/images/malfunction_indicator.png"
            Layout.preferredWidth: 45
            Layout.preferredHeight: 45
            fillMode: Image.PreserveAspectFit
        }


    }

    Rectangle {
        id: carRec
        width: 450
        height: 350
        x: 40
        y: (parent.height - carRec.height) / 2 + 60
        color: "transparent"

        Image {
            source: "qrc:/images/sport_car.png"
            width: parent.width
            height: parent.height
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            fillMode: Image.PreserveAspectFit
        }
    }

    Rectangle {
        id: speedRec
        width: 450
        height: 450
        x: 510
        y: (parent.height - speedRec.height) / 2 - 10
        color: "transparent"

        ColumnLayout {
            anchors.centerIn: parent
            spacing: 10

            Text {
                text: speed
                font.pixelSize: 120
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter
            }

            Text {
                text: "KM/H"
                font.pixelSize: 30
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter
            }

            Item {
                height: 5
            }

            Rectangle {
                id: barBg
                width: 350
                height: 10
                radius: 3
                color: "#AAAAAA"
                Layout.alignment: Qt.AlignHCenter

                Rectangle {
                    width: barBg.width * Math.min(speed / 300, 1)
                    height: barBg.height
                    radius: barBg.radius
//                    color: "#F7A5B9"
                    color: "#3CB371"
                }
            }
        }
    }

    Rectangle {
        id: turnSignalRec
        width: 430
        height: 100
        x: speedRec.x + (speedRec.width - turnSignalRec.width) / 2
        y: speedRec.y + 50
        color: "transparent"

        RowLayout {
            anchors.fill: parent
            anchors.margins: 10

            Image {
                source: "qrc:/images/left_arrow.png"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                fillMode: Image.PreserveAspectFit
                width: 48
                height: 48
            }

            Item {
                Layout.fillWidth: true
            }

            // Xi nhan phải (căn phải)
            Image {
                source: "qrc:/images/right_arrow.png"
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                fillMode: Image.PreserveAspectFit
                width: 48
                height: 48
            }
        }
    }

    Rectangle {
        id: gearRec
        width: 100
        height: 450
        x: 940
        y: (parent.height - gearRec.height) / 2
        color: "transparent"

        // Gear selector
        ColumnLayout {
            anchors.centerIn: parent
            spacing: 10

            Repeater {
                model: ["P", "R", "N", "D"]
                delegate: Text {
                    text: modelData
                    font.pixelSize: 32
                    color: gear === modelData ? "black" : "lightgray"
                    horizontalAlignment: Text.AlignRight
                }
            }
        }
    }

    Rectangle {
        id: bottomInfoBar
        width: parent.width
        height: 60
        color: "transparent"
        anchors.bottom: parent.bottom

        RowLayout {
            id: infoLayout
            anchors.fill: parent
            anchors.margins: 10
            spacing: 40

            // Time
            Text {
                text: time
                font.pixelSize: 24
                color: "black"
            }

            // Temperature
            Text {
                text: temp
                font.pixelSize: 24
                color: "black"
            }

            // Spacer
            Item { Layout.fillWidth: true }

            // Trip / Odometer
            Text {
                text: trip + "KM / " + odometer + "KM"
                font.pixelSize: 24
                color: "gray"
                horizontalAlignment: Text.AlignHCenter
            }

            // Spacer
            Item { Layout.fillWidth: true }

            // Battery status
            RowLayout {
                spacing: 8

                Rectangle {
                    width: 100
                    height: 24
                    radius: 3
                    color: "lightgray"

                    Rectangle {
                        width: parent.width * (battery / 100)
                        height: parent.height
                        radius: 3

                        color: {
                            if (battery > 50) return "green"
                            else if (battery > 20) return "orange"
                            else return "red"
                        }

                        Behavior on color {
                            ColorAnimation {
                                duration: 500
                            }
                        }
                    }
                }

                Text {
                    text: battery + "%"
                    font.pixelSize: 22
                    color: "black"
                    Layout.preferredWidth: 50
                    horizontalAlignment: Text.AlignLeft
                }
            }
        }
    }
}
