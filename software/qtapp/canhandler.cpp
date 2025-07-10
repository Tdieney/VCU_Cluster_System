#include "canhandler.h"
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <cstring>
#include <QDebug>

// Transmit thread
class CanTxThread : public QThread {
    Q_OBJECT
public:
    CanTxThread(QObject *parent = nullptr)
        : QThread(parent), m_socket(-1), m_running(false) {}

    ~CanTxThread() { stop(); }

    void enqueueMessage(const struct can_frame &frame) {
        QMutexLocker locker(&m_mutex);
        m_queue.enqueue(frame);
    }

    void stop() {
        m_running = false;
        wait();
        if (m_socket >= 0) close(m_socket);
    }

protected:
    void run() override {
        struct sockaddr_can addr;
        struct ifreq ifr;

        m_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (m_socket < 0) {
            qWarning() << "TX: Error opening CAN socket";
            return;
        }
        strcpy(ifr.ifr_name, "can0");
        if (ioctl(m_socket, SIOCGIFINDEX, &ifr) < 0) {
            qWarning() << "TX: Fail to specify CAN interface";
            return;
        }
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;
        if (bind(m_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            qWarning() << "TX: Error binding CAN socket";
            close(m_socket);
            return;
        }

        m_running = true;
        while (m_running) {
            m_mutex.lock();
            if (!m_queue.isEmpty()) {
                struct can_frame frame = m_queue.dequeue();
                m_mutex.unlock();
                int nbytes = write(m_socket, &frame, sizeof(frame));
                if (nbytes < 0) {
                    qWarning() << "TX: Error writing CAN frame";
                }
            } else {
                m_mutex.unlock();
                msleep(10);
            }
        }
        close(m_socket);
    }

private:
    int m_socket;
    bool m_running;
    QMutex m_mutex;
    QQueue<struct can_frame> m_queue;
};

// Receive thread
class CanRxThread : public QThread {
    Q_OBJECT
public:
    CanRxThread(QObject *parent = nullptr)
        : QThread(parent), m_socket(-1), m_running(false) {}

    ~CanRxThread() { stop(); }

    void stop() {
        m_running = false;
        wait();
        if (m_socket >= 0) close(m_socket);
    }

signals:
    void speedChanged(int speed);
    void batteryChanged(int battery);

protected:
    void run() override {
        struct sockaddr_can addr;
        struct ifreq ifr;
        struct can_frame rx_frame;

        m_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (m_socket < 0) {
            qWarning() << "RX: Error opening CAN socket";
            return;
        }
        strcpy(ifr.ifr_name, "can0");
        if (ioctl(m_socket, SIOCGIFINDEX, &ifr) < 0) {
            qWarning() << "RX: Fail to specify CAN interface";
            return;
        }
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;
        if (bind(m_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            qWarning() << "RX: Error binding CAN socket";
            close(m_socket);
            return;
        }

        m_running = true;
        while (m_running) {
            int nbytes = read(m_socket, &rx_frame, sizeof(struct can_frame));
            if (nbytes > 0) {
                if (rx_frame.can_id == 0x100) {
                    emit speedChanged(rx_frame.data[0]);
                }
                if (rx_frame.can_id == 0x101) {
                    emit batteryChanged(rx_frame.data[0]);
                }
            }
        }
        close(m_socket);
    }

private:
    int m_socket;
    bool m_running;
};

// CanHandler implementation
CanHandler::CanHandler(QObject *parent)
    : QObject(parent)
{
    // Set CAN parameters and bring up interface
    system(IP_CMD_SET_CAN0_PARAMS);
    system(IP_CMD_UP_CAN0);

    m_txThread = new CanTxThread(this);
    m_rxThread = new CanRxThread(this);

    connect(m_rxThread, &CanRxThread::speedChanged, this, &CanHandler::speedChanged);
    connect(m_rxThread, &CanRxThread::batteryChanged, this, &CanHandler::batteryChanged);

    m_txThread->start();
    m_rxThread->start();
}

CanHandler::~CanHandler() {
    m_txThread->stop();
    m_rxThread->stop();
}

void CanHandler::sendMessage(const struct can_frame &frame) {
    m_txThread->enqueueMessage(frame);
}
