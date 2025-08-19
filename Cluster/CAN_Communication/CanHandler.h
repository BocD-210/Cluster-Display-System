#pragma once
#include <QObject>
#include <QSocketNotifier>
#include <linux/can.h>

class CanHandler : public QObject {
    Q_OBJECT
public:
    explicit CanHandler(const QString &iface = "can0", QObject *parent = nullptr);
    ~CanHandler();

    bool isOpen() const { return m_sock >= 0; }

signals:
    void speedChanged(int kmh);
    void batteryChanged(int percent);

    void leftBlinkChanged(int val);
    void rightBlinkChanged(int val);
    void parkingLightChanged(int val);

// public slots:
//     void requestToggleLeft();
//     void requestToggleRight();
//     void requestToggleHazard();

private slots:
    void handleCanReadable();

private:
    int m_sock;
    QSocketNotifier *m_notifier;
    QString m_iface;

    int m_speed = 0;
    int m_battery = -1;

    int m_leftBlink = 0;
    int m_rightBlink = 0;
    int m_parkingLight = 0;
    
    bool openSocket();
    void closeSocket();
    void sendFrame(uint32_t can_id, const uint8_t *data, uint8_t dlc);
};
