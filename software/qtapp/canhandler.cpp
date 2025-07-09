#include "canhandler.h"
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <net/if.h>
#include <unistd.h>
#include <cstring>
#include <QDebug>

CanHandler::CanHandler(QObject *parent)
    : QThread(parent), m_socket(-1), m_running(false) {
    // Set CAN parameters
    system(IP_CMD_SET_CAN0_PARAMS);
    // Bring up the CAN interface
    system(IP_CMD_UP_CAN0);
    qDebug() << "CAN interface set up and brought up.";
}

CanHandler::~CanHandler() {
    stop();
}

void CanHandler::run() {
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    m_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (m_socket < 0) {
        qWarning() << "Error opening CAN socket";
        return;
    }

    strcpy(ifr.ifr_name, "can0");
    ioctl(m_socket, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(m_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        qWarning() << "Error binding CAN socket";
        close(m_socket);
        return;
    }

    m_running = true;
    while (m_running) {
        int nbytes = read(m_socket, &frame, sizeof(struct can_frame));
        if (nbytes > 0) {
            // Example: parse speed and battery from CAN frame
            if (frame.can_id == 0x100) {
                int speed = frame.data[0];
                emit speedChanged(speed);
            }
            if (frame.can_id == 0x101) {
                int battery = frame.data[0];
                emit batteryChanged(battery);
            }
        }
    }
    close(m_socket);
}

void CanHandler::stop() {
    m_running = false;
    if (m_socket >= 0)
    {
        close(m_socket);
        system(IP_CMD_DOWN_CAN0);
    }
}
