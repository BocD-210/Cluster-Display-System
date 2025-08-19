#include "CanHandler.h"
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <cstring>
#include <QDebug>

CanHandler::CanHandler(const QString &iface, QObject *parent)
    : QObject(parent), m_sock(-1), m_notifier(nullptr), m_iface(iface) {
    openSocket();
}

CanHandler::~CanHandler() {
    closeSocket();
}

bool CanHandler::openSocket() {
    struct ifreq ifr;
    struct sockaddr_can addr;

    m_sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (m_sock < 0) {
        perror("socket");
        return false;
    }

    std::strncpy(ifr.ifr_name, m_iface.toStdString().c_str(), IFNAMSIZ);
    if (ioctl(m_sock, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl");
        close(m_sock);
        m_sock = -1;
        return false;
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(m_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(m_sock);
        m_sock = -1;
        return false;
    }

    m_notifier = new QSocketNotifier(m_sock, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, &CanHandler::handleCanReadable);

    qDebug() << "CAN socket opened on" << m_iface;
    return true;
}

void CanHandler::closeSocket() {
    if (m_notifier) {
        m_notifier->deleteLater();
        m_notifier = nullptr;
    }
    if (m_sock >= 0) {
        close(m_sock);
        m_sock = -1;
    }
}

void CanHandler::handleCanReadable() {
    struct can_frame frame;
    int nbytes = read(m_sock, &frame, sizeof(frame));
    if (nbytes < 0) {
        perror("read");
        return;
    }

    switch (frame.can_id) {
    case 0x100: { // speed
        int spd = (frame.data[0] << 8) | frame.data[1]; 
        if (spd > 220) {
            spd = 200; // chặn max
        }
        if (spd != m_speed) {
            m_speed = spd;
            emit speedChanged(spd);
        }
        break;
    }

    case 0x101: { // battery
    
        int rawValue = (frame.data[0] << 8) | frame.data[1];

        int percent = (rawValue * 100) / 4095;

        if (percent != m_battery) {
            m_battery = percent;
            emit batteryChanged(percent);
        }
        break;
    }


    case 0x103: { // left turn signal
        int val = frame.data[0]; // 0 hoặc 1
        if (val != m_leftBlink) {
            m_leftBlink = val;
            emit leftBlinkChanged(val);
        }
        break;
    }

    case 0x104: { // right turn signal
        int val = frame.data[0];
        if (val != m_rightBlink) {
            m_rightBlink = val;
            emit rightBlinkChanged(val);
        }
        break;
    }

    case 0x105: { // Parking light
        int val = frame.data[0];
        if (val != m_parkingLight) {
            m_parkingLight = val;
            emit parkingLightChanged(val);
        }
        break;
    }
    default:
        break;
    }
}

void CanHandler::sendFrame(uint32_t can_id, const uint8_t *data, uint8_t dlc) {
    if (m_sock < 0) return;
    struct can_frame frame;
    frame.can_id = can_id;
    frame.can_dlc = dlc;
    memcpy(frame.data, data, dlc);
    write(m_sock, &frame, sizeof(frame));
}

