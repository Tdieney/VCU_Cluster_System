#include "canhandler.h"
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <cstring>
#include <QDebug>

digInSignal digInput;
digOutSignal digOutput;
analogInSignal analogInput;
uint64_t softTimer = 0;
uint16_t tick500ms = 0;
uint16_t prevTick500ms = 0xFFFF;
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

    if (obj.contains("digital_inputs")) {
        QJsonObject inputsObj = obj["digital_inputs"].toObject();
        for (const QString& key : inputsObj.keys()) {
            config.digInputs[key] = static_cast<uint8_t>(inputsObj[key].toInt());
            // qDebug() << "Input :" << key << "\t-----\t" << config.digInputs[key];
        }
    }

    if (obj.contains("analog_inputs")) {
        QJsonObject analogInputsObj = obj["analog_inputs"].toObject();
        for (const QString& key : analogInputsObj.keys()) {
            config.analogInputs[key] = static_cast<uint8_t>(analogInputsObj[key].toInt());
            // qDebug() << "Analog Input:" << key << "\t-----\t" << config.analogInputs[key];
        }
    }

    if (obj.contains("digital_outputs")) {
        QJsonObject outputsObj = obj["digital_outputs"].toObject();
        for (const QString& key : outputsObj.keys()) {
            config.digOutputs[key] = static_cast<uint8_t>(outputsObj[key].toInt());
            // qDebug() << "Output:" << key << "\t-----\t" << config.digOutputs[key];
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
    struct can_frame txFrame;

    txFrame.can_dlc = BYTES_PER_CAN_FRAME;
    
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
        if (tick500ms != prevTick500ms) {
            if (digOutput.left_front_light && digOutput.left_rear_light) {
                bool on = (tick500ms % 2 == 0);
                uint8_t left_front_val = on ? (0xC8 | digOutput.left_front_light) : 0xC8;
                uint8_t left_rear_val  = on ? (0xC8 | digOutput.left_rear_light)  : 0xC8;

                // Left front light frame
                txFrame.can_id = DIGITAL_OUTPUT_CMD_ID(digOutput.left_front_light_pos / DIGITAL_OUT_CMD_SIGNAL_PER_FRAME);
                memset(txFrame.data, 0, sizeof(txFrame.data));
                txFrame.data[digOutput.left_front_light_pos % DIGITAL_OUT_CMD_SIGNAL_PER_FRAME] = left_front_val;
                enqueueMessage(txFrame);

                // Left rear light frame
                txFrame.can_id = DIGITAL_OUTPUT_CMD_ID(digOutput.left_rear_light_pos / DIGITAL_OUT_CMD_SIGNAL_PER_FRAME);
                memset(txFrame.data, 0, sizeof(txFrame.data));
                txFrame.data[digOutput.left_rear_light_pos % DIGITAL_OUT_CMD_SIGNAL_PER_FRAME] = left_rear_val;
                enqueueMessage(txFrame);

                if (digInput.hazard_switch) {
                    emit hazardLightsChanged(on);
                } else if (digInput.turn_left_switch) {
                    emit leftLightChanged(on);
                }
            } else if (digOutput.left_front_light == false && digOutput.left_rear_light == false) {
                // Left front light frame
                txFrame.can_id = DIGITAL_OUTPUT_CMD_ID(digOutput.left_front_light_pos / DIGITAL_OUT_CMD_SIGNAL_PER_FRAME);
                memset(txFrame.data, 0, sizeof(txFrame.data));
                txFrame.data[digOutput.left_front_light_pos % DIGITAL_OUT_CMD_SIGNAL_PER_FRAME] = 0xC8;
                enqueueMessage(txFrame);

                // Left rear light frame
                txFrame.can_id = DIGITAL_OUTPUT_CMD_ID(digOutput.left_rear_light_pos / DIGITAL_OUT_CMD_SIGNAL_PER_FRAME);
                memset(txFrame.data, 0, sizeof(txFrame.data));
                txFrame.data[digOutput.left_rear_light_pos % DIGITAL_OUT_CMD_SIGNAL_PER_FRAME] = 0xC8;
                enqueueMessage(txFrame);

                emit hazardLightsChanged(false);
                emit leftLightChanged(false);
            }
            
            if (digOutput.right_front_light && digOutput.right_rear_light) {
                bool on = (tick500ms % 2 == 0);
                uint8_t right_front_val = on ? (0xC8 | digOutput.right_front_light) : 0xC8;
                uint8_t right_rear_val  = on ? (0xC8 | digOutput.right_rear_light)  : 0xC8;

                // Right front light frame
                txFrame.can_id = DIGITAL_OUTPUT_CMD_ID(digOutput.right_front_light_pos / DIGITAL_OUT_CMD_SIGNAL_PER_FRAME);
                memset(txFrame.data, 0, sizeof(txFrame.data));
                txFrame.data[digOutput.right_front_light_pos % DIGITAL_OUT_CMD_SIGNAL_PER_FRAME] = right_front_val;
                enqueueMessage(txFrame);

                // Right rear light frame
                txFrame.can_id = DIGITAL_OUTPUT_CMD_ID(digOutput.right_rear_light_pos / DIGITAL_OUT_CMD_SIGNAL_PER_FRAME);
                memset(txFrame.data, 0, sizeof(txFrame.data));
                txFrame.data[digOutput.right_rear_light_pos % DIGITAL_OUT_CMD_SIGNAL_PER_FRAME] = right_rear_val;
                enqueueMessage(txFrame);

                if (digInput.hazard_switch) {
                    emit hazardLightsChanged(on);
                } else if (digInput.turn_right_switch) {
                    emit rightLightChanged(on);
                }
            } else if (digOutput.right_front_light == false && digOutput.right_rear_light == false) {
                // Right front light frame
                txFrame.can_id = DIGITAL_OUTPUT_CMD_ID(digOutput.right_front_light_pos / DIGITAL_OUT_CMD_SIGNAL_PER_FRAME);
                memset(txFrame.data, 0, sizeof(txFrame.data));
                txFrame.data[digOutput.right_front_light_pos % DIGITAL_OUT_CMD_SIGNAL_PER_FRAME] = 0xC8;
                enqueueMessage(txFrame);

                // Right rear light frame
                txFrame.can_id = DIGITAL_OUTPUT_CMD_ID(digOutput.right_rear_light_pos / DIGITAL_OUT_CMD_SIGNAL_PER_FRAME);
                memset(txFrame.data, 0, sizeof(txFrame.data));
                txFrame.data[digOutput.right_rear_light_pos % DIGITAL_OUT_CMD_SIGNAL_PER_FRAME] = 0xC8;
                enqueueMessage(txFrame);

                emit hazardLightsChanged(false);
                emit rightLightChanged(false);
            }

            prevTick500ms = tick500ms;
        }


        m_mutex.lock();
        while (!m_queue.isEmpty()) {
            struct can_frame frame = m_queue.dequeue();
            m_mutex.unlock();

            int nbytes = write(m_socket, &frame, sizeof(frame));
            if (nbytes < 0) {
                qWarning() << "TX: Error writing CAN frame:" << strerror(errno);
            }

            m_mutex.lock();
        }
        m_mutex.unlock();
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
    digInSignal prevInput;
    uint8_t signalIdx = 0xFF;
    uint32_t signalCANID = 0;
    uint8_t signal_value = 0;
    int analogValue = 0;

    IOConfig config = loadIOConfig("io_configs/io_config.json"); // Load the IO configuration from a JSON file
    if (config.digInputs.isEmpty() || config.analogInputs.isEmpty() || config.digOutputs.isEmpty()) {
        qWarning() << "Failed to load IO configuration.";
        return; // Exit if configuration loading fails
    }

    for (auto it = config.digOutputs.constBegin(); it != config.digOutputs.constEnd(); ++it) {
        signalIdx = static_cast<uint8_t>(it.value());
        if (it.key() == "left_front_light") {
            digOutput.left_front_light_pos = signalIdx;
            qDebug() << "Left front light position set to:" << signalIdx;
        } else if (it.key() == "left_rear_light") {
            digOutput.left_rear_light_pos = signalIdx;
            qDebug() << "Left rear light position set to:" << signalIdx;
        } else if (it.key() == "right_front_light") {
            digOutput.right_front_light_pos = signalIdx;
            qDebug() << "Right front light position set to:" << signalIdx;
        } else if (it.key() == "right_rear_light") {
            digOutput.right_rear_light_pos = signalIdx;
            qDebug() << "Right rear light position set to:" << signalIdx;
        }
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
            for (auto it = config.digInputs.constBegin(); it != config.digInputs.constEnd(); ++it) {
                signalIdx = static_cast<uint8_t>(it.value());
                if (it.key() == "ignition") {
                    signalCANID = DIGITAL_INPUT_RES_ID(signalIdx / DIGITAL_IN_RESP_SIGNAL_PER_FRAME);
                    signal_value = rx_frame.data[signalIdx % DIGITAL_IN_RESP_SIGNAL_PER_FRAME] & 0x01;
                    if (rx_frame.can_id == signalCANID) {
                        if (digInput.ignition != signal_value) {
                            digInput.ignition = signal_value;
                            qDebug() << "Ignition status changed:" << digInput.ignition;
                        }
                    }
                }
                else if (it.key() == "turn_left_switch") {
                    if (digInput.ignition == false) continue;
                    signalCANID = DIGITAL_INPUT_RES_ID(signalIdx / DIGITAL_IN_RESP_SIGNAL_PER_FRAME);
                    signal_value = rx_frame.data[signalIdx % DIGITAL_IN_RESP_SIGNAL_PER_FRAME] & 0x01;
                    if (rx_frame.can_id == signalCANID) {
                        if (digInput.turn_left_switch != signal_value) {
                            digInput.turn_left_switch = signal_value;
                        }
                    }
                } else if (it.key() == "turn_right_switch") {
                    if (digInput.ignition == false) continue;
                    signalCANID = DIGITAL_INPUT_RES_ID(signalIdx / DIGITAL_IN_RESP_SIGNAL_PER_FRAME);
                    signal_value = rx_frame.data[signalIdx % DIGITAL_IN_RESP_SIGNAL_PER_FRAME] & 0x01;
                    if (rx_frame.can_id == signalCANID) {
                        if (digInput.turn_right_switch != signal_value) {
                            digInput.turn_right_switch = signal_value;
                        }
                    }
                } else if (it.key() == "hazard_switch") {
                    if (digInput.ignition == false) continue;
                    signalCANID = DIGITAL_INPUT_RES_ID(signalIdx / DIGITAL_IN_RESP_SIGNAL_PER_FRAME);
                    signal_value = rx_frame.data[signalIdx % DIGITAL_IN_RESP_SIGNAL_PER_FRAME] & 0x01;
                    if (rx_frame.can_id == signalCANID) {
                        if (digInput.hazard_switch != signal_value) {
                            digInput.hazard_switch = signal_value;
                        }
                    }
                } else if (it.key() == "high_beam_switch") {
                    if (digInput.ignition == false) continue;
                    signalCANID = DIGITAL_INPUT_RES_ID(signalIdx / DIGITAL_IN_RESP_SIGNAL_PER_FRAME);
                    signal_value = rx_frame.data[signalIdx % DIGITAL_IN_RESP_SIGNAL_PER_FRAME] & 0x01;
                    if (rx_frame.can_id == signalCANID) {
                        if (digInput.high_beam_switch != signal_value) {
                            digInput.high_beam_switch = signal_value;
                            emit highBeamChanged(digInput.high_beam_switch);
                        }
                    }
                } else if (it.key() == "low_beam_switch") {
                    if (digInput.ignition == false) continue;
                    signalCANID = DIGITAL_INPUT_RES_ID(signalIdx / DIGITAL_IN_RESP_SIGNAL_PER_FRAME);
                    signal_value = rx_frame.data[signalIdx % DIGITAL_IN_RESP_SIGNAL_PER_FRAME] & 0x01;
                    if (rx_frame.can_id == signalCANID) {
                        if (digInput.low_beam_switch != signal_value) {
                            digInput.low_beam_switch = signal_value;
                            emit lowBeamChanged(digInput.low_beam_switch);
                        }
                    }
                } else if (it.key() == "parking_lights_switch") {
                    if (digInput.ignition == false) continue;
                    signalCANID = DIGITAL_INPUT_RES_ID(signalIdx / DIGITAL_IN_RESP_SIGNAL_PER_FRAME);
                    signal_value = rx_frame.data[signalIdx % DIGITAL_IN_RESP_SIGNAL_PER_FRAME] & 0x01;
                    if (rx_frame.can_id == signalCANID) {
                        if (digInput.parking_lights_switch != signal_value) {
                            digInput.parking_lights_switch = signal_value;
                            emit parkingLightsChanged(digInput.parking_lights_switch);
                        }
                    }
                }

                if (digInput.hazard_switch && digInput.hazard_switch != prevInput.hazard_switch) {
                    softTimer = 0;
                    tick500ms = 0;
                    prevTick500ms = 0xFFFF;
                    digOutput.left_front_light = true;
                    digOutput.left_rear_light = true;
                    digOutput.right_front_light = true;
                    digOutput.right_rear_light = true;
                } else if (digInput.turn_left_switch && digInput.turn_left_switch != prevInput.turn_left_switch) {
                    softTimer = 0;
                    tick500ms = 0;
                    prevTick500ms = 0xFFFF;
                    digOutput.left_front_light = true;
                    digOutput.left_rear_light = true;
                    digOutput.right_front_light = false;
                    digOutput.right_rear_light = false;
                } else if (digInput.turn_right_switch && digInput.turn_right_switch != prevInput.turn_right_switch) {
                    softTimer = 0;
                    tick500ms = 0;
                    prevTick500ms = 0xFFFF;
                    digOutput.left_front_light = false;
                    digOutput.left_rear_light = false;
                    digOutput.right_front_light = true;
                    digOutput.right_rear_light = true;
                } else if ((digInput.hazard_switch == false && digInput.turn_left_switch == false && digInput.turn_right_switch == false) && 
                           (prevInput.hazard_switch || prevInput.turn_left_switch || prevInput.turn_right_switch)) {
                    softTimer = 0;
                    tick500ms = 0;
                    prevTick500ms = 0xFFFF;
                    digOutput.left_front_light = false;
                    digOutput.left_rear_light = false;
                    digOutput.right_front_light = false;
                    digOutput.right_rear_light = false;
                }
            }

            for (auto it = config.analogInputs.constBegin(); it != config.analogInputs.constEnd(); ++it) {
                signalIdx = static_cast<uint8_t>(it.value());
                if (it.key() == "speed") {
                    signalCANID = ANALOG_INPUT_RES_ID(signalIdx / ANALOG_IN_RESP_SIGNAL_PER_FRAME);
                    analogValue = ((rx_frame.data[(signalIdx % ANALOG_IN_RESP_SIGNAL_PER_FRAME) + 1] & 0xCF) << 8) | (rx_frame.data[signalIdx % ANALOG_IN_RESP_SIGNAL_PER_FRAME] & 0xFF);
                    if (rx_frame.can_id == signalCANID) {
                        if (analogInput.speed != analogValue) {
                            analogInput.speed = analogValue;
                            // qDebug() << "Speed status changed:" << analogInput.speed;
                            emit speedChanged(analogInput.speed);
                        }
                    }
                }
            }
        }

        memcpy(&prevInput, &digInput, sizeof(digInSignal));
    }
    close(m_socket);
}

DataProcessing::DataProcessing() {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DataProcessing::DataProcessingTask);
    timer->start(1); // 1 millisecond interval
}

void DataProcessing::DataProcessingTask() {
    tick500ms = softTimer / 500;
    softTimer++;
}

CanHandler::CanHandler(QObject *parent)
    : QObject(parent)
{
    m_txThread = new CanTxThread(this);
    m_rxThread = new CanRxThread(this);
    m_dataProcessing = new DataProcessing();

    connect(m_txThread, &CanTxThread::leftLightChanged, this, &CanHandler::leftLightChanged);
    connect(m_txThread, &CanTxThread::rightLightChanged, this, &CanHandler::rightLightChanged);
    connect(m_txThread, &CanTxThread::hazardLightsChanged, this, &CanHandler::hazardLightsChanged);
    connect(m_rxThread, &CanRxThread::highBeamChanged, this, &CanHandler::highBeamChanged);
    connect(m_rxThread, &CanRxThread::lowBeamChanged, this, &CanHandler::lowBeamChanged);
    connect(m_rxThread, &CanRxThread::parkingLightsChanged, this, &CanHandler::parkingLightsChanged);
    connect(m_rxThread, &CanRxThread::speedChanged, this, &CanHandler::speedChanged);

    m_txThread->start();
    m_rxThread->start();
}

CanHandler::~CanHandler() {
    m_txThread->stop();
    m_rxThread->stop();
}
