#ifndef CANHANDLER_H
#define CANHANDLER_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QMutex>
#include <QQueue>
#include <linux/can.h>

#define IP_CMD_SET_CAN0_PARAMS  "ip link set can0 type can bitrate 666666"
#define IP_CMD_UP_CAN0          "ip link set can0 up"
#define IP_CMD_DOWN_CAN0        "ip link set can0 down"

class CanTxThread;
class CanRxThread;

class CanHandler : public QObject {
    Q_OBJECT
public:
    explicit CanHandler(QObject *parent = nullptr);
    ~CanHandler();

    void sendMessage(const struct can_frame &frame);

signals:
    void speedChanged(int speed);
    void batteryChanged(int battery);

private:
    CanTxThread *m_txThread;
    CanRxThread *m_rxThread;
};

#endif // CANHANDLER_H
