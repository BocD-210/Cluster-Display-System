// main.qml
import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import "./"

Window {
    id : root
    width: 1920
    height: 960
    visible: true
    title: qsTr("Car DashBoard")
    color: "#1E1E1E"
    visibility: "FullScreen"

    Shortcut {
        sequence: "Ctrl+Q"
        context: Qt.ApplicationShortcut
        onActivated: Qt.quit()
    }

    Timer {
        interval: 500
        running: true
        repeat: true
        onTriggered:{

            currentTime.text = Qt.formatDateTime(new Date(), "hh:mm")
        }
    }

    property string temperatureText: ""
    property int batteryPercent: 100

    Rectangle {
        anchors.fill: parent
        color: "black"
        border.color: "black"
        radius: 50

        Rectangle{
            id: topbar
            width: parent.width
            height: 100
            color: "black"
            radius: parent.radius
            antialiasing: true
            clip: true

            Label {
                id: currentTime
                text: Qt.formatDateTime(new Date(), "hh:mm")
                font.pixelSize: 32
                font.family: "Inter"
                font.bold: Font.DemiBold
                color: "#FFFFFF"
                anchors.left: parent.left
                anchors.leftMargin: 190 
                anchors.top: parent.top
                anchors.topMargin: 25
            }

            Label {
                id: currentDate
                text: Qt.formatDateTime(new Date(), "dd/MM/yyyy")
                font.pixelSize: 32
                font.family: "Inter"
                font.bold: Font.DemiBold
                color: "#FFFFFF"
                anchors.right: parent.right
                anchors.rightMargin: 190   
                anchors.top: parent.top
                anchors.topMargin: 25
            }

            RowLayout {
                anchors.fill: parent
                spacing: 20

                Item { Layout.fillWidth: true }

                RowLayout {
                    spacing: 100

                    Image {
                        id: leftSignal
                        source: "qrc:/assets/turn_left.png"
                        width: 30
                        height: 30
                        visible: true
                        opacity: 0.3
                    }


                    Image {
                        id: headLight
                        property bool indicator: false
                        source: indicator ? "qrc:/assets/Low_beam_headlights.svg" : "qrc:/assets/Low_beam_headlights_white.svg"
                        Behavior on indicator {NumberAnimation { duration: 300}}
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                headLight.indicator = !headLight.indicator
                            }
                        }
                    }

                    
                    Image {
                        id: parkLight
                        property bool indicator: false
                        source: indicator ? "qrc:/assets/Parking_lights.svg" 
                                        : "qrc:/assets/Parking_lights_white.svg"
                        Behavior on indicator { NumberAnimation { duration: 300 } }
                    }

                    Image {
                        id: fogLight
                        property bool indicator: false
                        source: indicator ? "qrc:/assets/Rare_fog_lights_red.svg" : "qrc:/assets/Rare_fog_lights.svg"
                        Behavior on indicator {NumberAnimation { duration: 300}}
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                fogLight.indicator = !fogLight.indicator
                            }
                        }
                    }

                    
                    Image {
                        id: rightSignal
                        source: "qrc:/assets/turn_right.png"
                        width: 30
                        height: 30
                        visible: true
                        opacity: 0.3 
                    }

                }
                Item {Layout.fillWidth: true}

            }

            Rectangle{
                width: parent.width * 0.4
                height: 0.5
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
            }
        }

        Rectangle {
            id: middleContent
            width: parent.width
            height: parent.height -topbar.height -botbar.height
            anchors.top: topbar.bottom
            color: "transparent"

            RowLayout {
                anchors.fill: parent
                spacing: 0

                Rectangle {
                    id: leftSection
                    Layout.preferredWidth: parent.width / 3
                    Layout.fillHeight: true
                    color: "black"

                    BatteryGauge {
                        anchors.centerIn: parent
                        batteryLevel: batteryPercent
                    }
                }

                Rectangle {
                    id: centerSection
                    Layout.preferredWidth: parent.width / 3
                    Layout.fillHeight: true
                    color: "black"

                    Image {
                        id: leftRoad
                        width: 127
                        height: 500
                        anchors {
                            left: parent.left
                            leftMargin: 60
                            bottom: parent.bottom
                            bottomMargin: 26.5
                        }
                        source: "qrc:/assets/Vector_1.svg"
                    }

                    Image {
                        id: rightRoad
                        width: 127
                        height: 500
                        anchors {
                            right: parent.right
                            rightMargin: 60
                            bottom: parent.bottom
                            bottomMargin: 26.5
                        }
                        source: "qrc:/assets/Vector_2.svg"
                    }

                    Image {
                        id: car
                        source: "qrc:/assets/Car.svg"
                        width: 200  
                        height: 200
                        fillMode: Image.PreserveAspectFit

                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: 130   
                        }
                    }

                    Image {
                        source: "qrc:/assets/Model_3.png"
                        width: 160
                        height: 160

                        anchors {
                            bottom: car.top
                            bottomMargin: 50
                            horizontalCenter: car.horizontalCenter
                        }
                    }

                }

                Rectangle {
                    id: rightSection
                    Layout.preferredWidth: parent.width / 3
                    Layout.fillHeight: true
                    color: "black"

                    property int speed: 0

                    Column {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.horizontalCenterOffset: -100 
                        spacing: 6

                        Text {
                            id: speedText
                            text: "0"
                            font.pixelSize: 100
                            font.bold: true
                            font.family: "Arial"
                            color: "white"
                            horizontalAlignment: Text.AlignHCenter
                        }
                        Text {
                            text: "KM/H"
                            font.pixelSize: 65
                            font.bold: true
                            color: "#CCCCCC" 
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }

                    Item {
                        id: rightIndicatorsGroup
                        width: parent.width
                        height: parent.height
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.top: parent.top  
                        anchors.leftMargin: 350 
                        anchors.topMargin: 170  

                        Image {
                            id: forthRightIndicator
                            property bool indicator: true
                            width: 70
                            height: 45
                            anchors.left: parent.left
                            anchors.leftMargin: 80
                            anchors.top: parent.top
                            anchors.topMargin: 0
                            source: indicator ? "qrc:/assets/FourthRightIcon.svg" : "qrc:/assets/FourthRightIcon_red.svg"
                            MouseArea {
                                anchors.fill: parent
                                onClicked: forthRightIndicator.indicator = !forthRightIndicator.indicator
                            }
                        }

                        Image {
                            id: thirdRightIndicator
                            property bool indicator: true
                            width: 70
                            height: 45
                            anchors.left: parent.left
                            anchors.leftMargin: 110
                            anchors.top: forthRightIndicator.bottom
                            anchors.topMargin: 35
                            source: indicator ? "qrc:/assets/thirdRightIcon.svg" : "qrc:/assets/thirdRightIcon_red.svg"
                            MouseArea {
                                anchors.fill: parent
                                onClicked: thirdRightIndicator.indicator = !thirdRightIndicator.indicator
                            }
                        }

                        Image {
                            id: secondRightIndicator
                            property bool indicator: true
                            width: 70
                            height: 45
                            anchors.left: parent.left
                            anchors.leftMargin: 140
                            anchors.top: thirdRightIndicator.bottom
                            anchors.topMargin: 35
                            source: indicator ? "qrc:/assets/SecondRightIcon.svg" : "qrc:/assets/SecondRightIcon_red.svg"
                            MouseArea {
                                anchors.fill: parent
                                onClicked: secondRightIndicator.indicator = !secondRightIndicator.indicator
                            }
                        }

                        Image {
                            id: firstRightIndicator
                            property bool sheetBelt: true
                            width: 50
                            height: 45
                            anchors.left: parent.left
                            anchors.leftMargin: 170
                            anchors.top: secondRightIndicator.bottom
                            anchors.topMargin: 35
                            source: sheetBelt ? "qrc:/assets/FirstRightIcon.svg" : "qrc:/assets/FirstRightIcon_grey.svg"
                            MouseArea {
                                anchors.fill: parent
                                onClicked: firstRightIndicator.sheetBelt = !firstRightIndicator.sheetBelt
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            id: botbar
            width: parent.width
            height: 100
            color: "black"
            anchors.bottom: parent.bottom 
            radius: parent.radius
            antialiasing: true
            clip: true

            Timer {
            interval: 2000
            running: true
            repeat: true
            onTriggered: {
                var temp = 15 + Math.floor(Math.random() * 25);
                if (temp < 20)
                    temperatureText = "Temporature: " + temp + "°C";
                else if (temp < 30)
                    temperatureText = "Temporature: " + temp + "°C";
                else if (temp < 34)
                    temperatureText = "Temporature: " + temp + "°C";
                else
                    temperatureText = "Temporature: " + temp + "°C";
            }
        }

        Component.onCompleted: {
            temperatureText = "Temporature: 32°C";
        }

        Text {
            id: temperatureDisplay
            text: temperatureText
            font.pixelSize: 36
            font.bold: true
            color: {
                var temp = parseInt(temperatureText.match(/\d+/));
                if (temp < 20)
                    return "#87CEFA";
                else if (temp < 30)
                    return "#50C878";
                else if (temp < 34)
                    return "orange";
                else
                    return "red";
            }
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.verticalCenter: parent.verticalCenter
        }

            Row {
                id: statusRow
                spacing: 45
                anchors.centerIn: parent

                Text {
                    text: "READY"
                    color: "#50C878"
                    font.bold: true
                    font.pixelSize: 40
                }

                Text {
                    text: "P"
                    color: "#444444"
                    font.bold: true
                    font.pixelSize: 40
                }
                Text {
                    text: "N"
                    color: "#444444"
                    font.bold: true
                    font.pixelSize: 40
                }
                Text {
                    text: "R"
                    color: "#444444"
                    font.bold: true
                    font.pixelSize: 40
                }
                Text {
                    text: "D"
                    color: "white"
                    font.bold: true
                    font.pixelSize: 40
                }
            }
        }
    }

    Connections {
        target: canHandler

        onBatteryChanged: {
            batteryPercent = percent
        }
        onSpeedChanged: function(val) {
            speedText.text = val.toString()
        }
        onLeftBlinkChanged: {
            if (val === 1) {
                leftSignal.opacity = 1.0
            } else {
                leftSignal.opacity = 0.3
            }
        }
        onRightBlinkChanged: {
            if (val === 1) {
                rightSignal.opacity = 1.0
            } else {
                rightSignal.opacity = 0.3
            }
        }
        onParkingLightChanged: {
            parkLight.indicator = (val === 1)
        }
    }

}
