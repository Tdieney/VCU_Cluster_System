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
#include <QTimer>
#include <QTimer>

// CAN IDs
#define DIGITAL_OUTPUT_CMD_ID(n)          (0x94FF0000UL + ((n) * 0x20UL))
#define DIGITAL_OUTPUT_RES_ID(n)          (0x94FF0800UL + ((n) * 0x20UL))
#define DIGITAL_INPUT_RES_ID(n)           (0x94FF0A00UL + ((n) * 0x20UL))
#define ANALOG_INPUT_RES_ID(n)            (0x94FF0D00UL + ((n) * 0x20UL))

#define NUMBER_OF_DIG_OUT_CMD_FRAME       4U
#define NUMBER_OF_DIG_OUT_RES_FRAME       8U
#define NUMBER_OF_DIG_IN_RES_FRAME        4U
#define NUMBER_OF_ANALOG_IN_RES_FRAME     8U

// CAN Frame Signal Per Frame
#define DIGITAL_OUT_CMD_SIGNAL_PER_FRAME  8U
#define DIGITAL_OUT_RESP_SIGNAL_PER_FRAME 4U
#define DIGITAL_IN_RESP_SIGNAL_PER_FRAME  8U
#define ANALOG_IN_RESP_SIGNAL_PER_FRAME   4U

#define BYTES_PER_CAN_FRAME               8U

#define ANALOG_VALUE_BITS                 14U
#define ANALOG_EL_DIAGNOSIS_BITS          2U

/*
 * @brief Analog Input Response (ECU -> VCU)
 */
typedef struct
{
  uint16_t analogValue   : ANALOG_VALUE_BITS;
  uint8_t elDiagnosis    : ANALOG_EL_DIAGNOSIS_BITS;
} AnalogInput_Resp;

typedef union {
  uint64_t sdu;
  AnalogInput_Resp signal[ANALOG_IN_RESP_SIGNAL_PER_FRAME];
} AnalogInput_Resp_Frame;

struct IOConfig {
    QMap<QString, uint8_t> digInputs;
    QMap<QString, int> analogInputs;
    QMap<QString, uint8_t> digOutputs;
};

struct digInSignal {
    bool ignition = false;
    bool turn_left_switch = false;
    bool turn_right_switch = false;
    bool hazard_switch = false;
    bool high_beam_switch = false;
    bool low_beam_switch = false;
    bool parking_lights_switch = false;
};

struct digOutSignal {
    bool left_front_light = false;
    bool left_rear_light = false;
    bool right_front_light = false;
    bool right_rear_light = false;
    uint8_t left_front_light_pos = 0;
    uint8_t left_rear_light_pos = 0;
    uint8_t right_front_light_pos = 0;
    uint8_t right_rear_light_pos = 0;
};

struct analogInSignal {
    int speed = 0; 
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

signals:
    void leftLightChanged(bool leftLight);
    void rightLightChanged(bool rightLight);
    void hazardLightsChanged(bool hazardLights);
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
    void highBeamChanged(bool highBeam);
    void lowBeamChanged(bool lowBeam);
    void parkingLightsChanged(bool parkingLights);
    void speedChanged(int analogVal);

private:
    int m_socket;
    bool m_running;
};

class DataProcessing : public QObject {
    Q_OBJECT
public:
    DataProcessing();
    QTimer *timer;
public slots:
    void DataProcessingTask();
};

class CanHandler : public QObject {
    Q_OBJECT
public:
    explicit CanHandler(QObject *parent = nullptr);
    ~CanHandler();

signals:
    void leftLightChanged(bool leftLight);
    void rightLightChanged(bool rightLight);
    void hazardLightsChanged(bool hazardLights);
    void highBeamChanged(bool highBeam);
    void lowBeamChanged(bool lowBeam);
    void parkingLightsChanged(bool parkingLights);
    void speedChanged(int analogVal);

private:
    CanTxThread *m_txThread;
    CanRxThread *m_rxThread;
    DataProcessing *m_dataProcessing;
};

#endif // CANHANDLER_H
