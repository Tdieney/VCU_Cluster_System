#ifndef CANHANDLER_H
#define CANHANDLER_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QMutex>
#include <QQueue>
#include <linux/can.h>
#include <cstdint>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#define IP_CMD_SET_CAN0_PARAMS  "sudo ip link set can0 type can bitrate 666666"
#define IP_CMD_UP_CAN0          "sudo ip link set can0 up"
#define IP_CMD_DOWN_CAN0        "sudo ip link set can0 down"

// CAN IDs
#define DIGITAL_OUTPUT_CMD_ID(n)          (0x94FF0000UL + ((n) * 0x20UL))
#define DIGITAL_OUTPUT_RES_ID(n)          (0x94FF0800UL + ((n) * 0x20UL))
#define DIGITAL_INPUT_RES_ID(n)           (0x94FF0A00UL + ((n) * 0x20UL))

#define NUMBER_OF_DIG_OUT_CMD_FRAME       4U
#define NUMBER_OF_DIG_OUT_RES_FRAME       8U
#define NUMBER_OF_DIG_IN_RES_FRAME        4U

// CAN Frame Signal Per Frame
#define DIGITAL_OUT_CMD_SIGNAL_PER_FRAME  8U
#define DIGITAL_OUT_RESP_SIGNAL_PER_FRAME 4U
#define DIGITAL_IN_RESP_SIGNAL_PER_FRAME  8U

struct IOConfig {
    QMap<QString, uint8_t> inputs;
    QMap<QString, uint8_t> outputs;
};

struct inputSignal {
    bool ignition = false;
    bool turn_left_switch = false;
    bool turn_right_switch = false;
    bool hazard_switch = false;
};

struct outputSignal {
    bool left_front_light = false;
    bool left_rear_light = false;
    bool right_front_light = false;
    bool right_rear_light = false;
};

/*
 * @brief Load the IO configuration from a JSON file.
 * @param path: The path to the JSON file containing the IO configuration.
 * @return IOConfig object containing the loaded configuration.
 */
IOConfig loadIOConfig(const QString& path);

class CanTxThread : public QThread {
    Q_OBJECT
public:
    CanTxThread(QObject *parent = nullptr);
    ~CanTxThread();

    void enqueueMessage(const struct can_frame &frame);
    void stop();

protected:
    void run() override;

private:
    int m_socket;
    bool m_running;
    QMutex m_mutex;
    QQueue<struct can_frame> m_queue;
};

class CanRxThread : public QThread {
    Q_OBJECT
public:
    CanRxThread(QObject *parent = nullptr);
    ~CanRxThread();

    void stop();

protected:
    void run() override;

signals:
    void leftLightChanged(bool leftLight);
    void rightLightChanged(bool rightLight);

private:
    int m_socket;
    bool m_running;
};

class CanHandler : public QObject {
    Q_OBJECT
public:
    explicit CanHandler(QObject *parent = nullptr);
    ~CanHandler();

    void sendMessage(const struct can_frame &frame);

signals:
    void leftLightChanged(bool leftLight);
    void rightLightChanged(bool rightLight);

private:
    CanTxThread *m_txThread;
    CanRxThread *m_rxThread;
};

#endif // CANHANDLER_H
