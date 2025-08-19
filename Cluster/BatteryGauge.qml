// BatteryGauge.qml
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Shapes 1.14

Item {
    id: batteryGauge
    width: 400    // tăng kích thước tổng thể
    height: 400

    property int batteryLevel: 100

    Canvas {
        id: gaugeCanvas
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)

            var centerX = width / 2
            var centerY = height / 2
            var lineWidth = 30     // vẫn hợp lý

            var radius = (Math.min(width, height) - lineWidth) / 2  // auto fit
            var startAngle = -Math.PI / 2
            var endAngle = startAngle + (batteryLevel / 100) * 2 * Math.PI

            // Vòng nền
            ctx.beginPath()
            ctx.arc(centerX, centerY, radius, 0, 2 * Math.PI)
            ctx.lineWidth = lineWidth
            ctx.strokeStyle = "#444"
            ctx.stroke()

            // Vòng màu pin
            ctx.beginPath()
            ctx.arc(centerX, centerY, radius, startAngle, endAngle)
            ctx.lineWidth = lineWidth
            ctx.strokeStyle = batteryLevel > 50 ? "#00FF00" : batteryLevel > 20 ? "yellow" : "red"
            ctx.stroke()
        }

        Connections {
            target: batteryGauge
            onBatteryLevelChanged: gaugeCanvas.requestPaint()
        }
    }

    Label {
        text: "Battery Charge"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -40
        font.pixelSize: 37
        font.bold: true
        color: "white"
    }

    Label {
        text: batteryLevel + "%"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 30
        font.pixelSize: 40
        color: "#CCCCCC"
    }
}

