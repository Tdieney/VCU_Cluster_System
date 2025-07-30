#include "canhandler.h"
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <cstring>
#include <QDebug>

inputSignal input;
outputSignal output;

/*
 * @brief Load the IO configuration from a JSON file.
 * @param path: The path to the JSON file containing the IO configuration.
 * @return IOConfig object containing the loaded configuration.
 */
IOConfig loadIOConfig(const QString& path) {
    IOConfig config;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file:" << path;
        return config;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return config;
    }

    QJsonObject obj = doc.object();

    if (obj.contains("inputs")) {
        QJsonObject inputsObj = obj["inputs"].toObject();
        for (const QString& key : inputsObj.keys()) {
            config.inputs[key] = static_cast<uint8_t>(inputsObj[key].toInt());
            // qDebug() << "Input :" << key << "\t-----\t" << config.inputs[key];
        }
    }

    if (obj.contains("outputs")) {
        QJsonObject outputsObj = obj["outputs"].toObject();
        for (const QString& key : outputsObj.keys()) {
            config.outputs[key] = static_cast<uint8_t>(outputsObj[key].toInt());
            // qDebug() << "Output:" << key << "\t-----\t" << config.outputs[key];
        }
    }

    return config;
}

CanTxThread::CanTxThread(QObject *parent)
        : QThread(parent), m_socket(-1), m_running(false) {}

CanTxThread::~CanTxThread() { stop(); }

void CanTxThread::enqueueMessage(const struct can_frame &frame) {
    QMutexLocker locker(&m_mutex);
    m_queue.enqueue(frame);
}

void CanTxThread::stop() {
    m_running = false;
    wait();

    QMutexLocker locker(&m_mutex);
    m_queue.clear();

    if (m_socket >= 0) {
        close(m_socket);
        m_socket = -1;
    }
}

void CanTxThread::run() {
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
                qWarning() << "TX: Error writing CAN frame:" << strerror(errno);
            }
        } else {
            m_mutex.unlock();
        }
    }
    close(m_socket);
}

CanRxThread::CanRxThread(QObject *parent)
        : QThread(parent), m_socket(-1), m_running(false) {}

CanRxThread::~CanRxThread() {
    stop();
}

void CanRxThread::stop() {
    m_running = false;
    wait();

    if (m_socket >= 0) {
        close(m_socket);
        m_socket = -1;
    }
}

void CanRxThread::run() {
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame rx_frame;
    uint8_t signal_idx = 0xFF;
    uint32_t signal_canid = 0;
    uint8_t signal_value = 0;
    
    IOConfig config = loadIOConfig("configs/io_config.json"); // Load the IO configuration from a JSON file
    if (config.inputs.isEmpty() || config.outputs.isEmpty()) {
        qWarning() << "Failed to load IO configuration.";
        return; // Exit if configuration loading fails
    }

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
            // Check for input responses
            for (auto it = config.inputs.constBegin(); it != config.inputs.constEnd(); ++it) {
                signal_idx = static_cast<uint8_t>(it.value());
                if (it.key() == "ignition") {
                    signal_canid = DIGITAL_INPUT_RES_ID(signal_idx / DIGITAL_IN_RESP_SIGNAL_PER_FRAME);
                    signal_value = rx_frame.data[signal_idx % DIGITAL_IN_RESP_SIGNAL_PER_FRAME] & 0x01;
                    if (rx_frame.can_id == signal_canid) {
                        if (input.ignition != signal_value) {
                            input.ignition = signal_value;
                            qDebug() << "Ignition status changed:" << input.ignition;
                        }
                    }
                }
                else if (it.key() == "turn_left_switch") {
                    signal_canid = DIGITAL_INPUT_RES_ID(signal_idx / DIGITAL_IN_RESP_SIGNAL_PER_FRAME);
                    signal_value = rx_frame.data[signal_idx % DIGITAL_IN_RESP_SIGNAL_PER_FRAME] & 0x01;
                    // qDebug() << "ID:" << signal_canid << "Value:" << signal_value;
                    if (rx_frame.can_id == signal_canid) {
                        if (input.turn_left_switch != signal_value) {
                            input.turn_left_switch = signal_value;
                            qDebug() << "Turn left switch status changed:" << input.turn_left_switch;
                        }
                    }
                } else if (it.key() == "turn_right_switch") {
                    signal_canid = DIGITAL_INPUT_RES_ID(signal_idx / DIGITAL_IN_RESP_SIGNAL_PER_FRAME);
                    signal_value = rx_frame.data[signal_idx / DIGITAL_IN_RESP_SIGNAL_PER_FRAME] & 0x01;
                    if (rx_frame.can_id == signal_canid) {
                        if (input.turn_right_switch != signal_value) {
                            input.turn_right_switch = signal_value;
                            qDebug() << "Turn right switch status changed:" << input.turn_right_switch;
                        }
                    }
                } else if (it.key() == "hazard_switch") {
                    signal_canid = DIGITAL_INPUT_RES_ID(signal_idx / DIGITAL_IN_RESP_SIGNAL_PER_FRAME);
                    signal_value = rx_frame.data[signal_idx / DIGITAL_IN_RESP_SIGNAL_PER_FRAME] & 0x01;
                    if (rx_frame.can_id == signal_canid) {
                        if (input.hazard_switch != signal_value) {
                            input.hazard_switch = signal_value;
                            qDebug() << "Hazard switch status changed:" << input.hazard_switch;
                        }
                    }
                }
            }
            // if (rx_frame.can_id == 0x94FF0A00) {
            //     // frame.data[0]
            // }
        }
    }
    close(m_socket);
}

CanHandler::CanHandler(QObject *parent)
    : QObject(parent)
{
    // Set CAN parameters and bring up interface
    system(IP_CMD_SET_CAN0_PARAMS);
    system(IP_CMD_UP_CAN0);

    m_txThread = new CanTxThread(this);
    m_rxThread = new CanRxThread(this);

    connect(m_rxThread, &CanRxThread::leftLightChanged, this, &CanHandler::leftLightChanged);
    connect(m_rxThread, &CanRxThread::rightLightChanged, this, &CanHandler::rightLightChanged);

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
