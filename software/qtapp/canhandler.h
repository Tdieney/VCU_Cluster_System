#ifndef CANHANDLER_H
#define CANHANDLER_H

#include <QObject>
#include <QThread>
#include <QString>

#define IP_CMD_SET_CAN0_PARAMS  "ip link set can0 type can bitrate 666666"
#define IP_CMD_UP_CAN0          "ip link set can0 up"
#define IP_CMD_DOWN_CAN0        "ip link set can0 down"

class CanHandler : public QThread {
    Q_OBJECT
public:
    explicit CanHandler(QObject *parent = nullptr);
    ~CanHandler();
    void run() override;
    void stop();

signals:
    void speedChanged(int speed);
    void batteryChanged(int battery);

private:
    int m_socket;
    bool m_running;
};

#endif // CANHANDLER_H
