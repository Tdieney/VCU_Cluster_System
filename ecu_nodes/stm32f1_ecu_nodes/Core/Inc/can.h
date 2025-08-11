/*
 * can.h
 *
 *  Created on: Jul 12, 2025
 *      Author: Tdieney
 */

#ifndef INC_CAN_H_
#define INC_CAN_H_

#include <stdint.h>
#include <stdbool.h>
#include "main.h"

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

// Maximum number of signals per type
#define MAX_DIG_OUT_CMD_SIGNALS           32U
#define MAX_DIG_OUT_RESP_SIGNALS          32U
#define MAX_DIG_IN_RESP_SIGNALS           32U
#define MAX_ANALOG_IN_RESP_SIGNALS        32U

// Bitfield widths for Digital Output/Tx/Inputs
#define SWITCH_CMD_BITS                   1U
#define DUTY_CYCLE_BITS                   7U
#define STATUS_PS_BITS                    1U
#define OUTPUT_EL_DIAGNOSIS_BITS          3U
#define CURRENT_FB_BITS                   12U
#define INPUT_STATUS_BITS                 1U
#define FRESHNESS_BITS                    5U
#define INPUT_EL_DIAGNOSIS_BITS           2U
#define ANALOG_VALUE_BITS                 14U
#define ANALOG_EL_DIAGNOSIS_BITS          2U

#define DIGITAL_OUT_CMD_SWITCH_ON         0x01U
#define DIGITAL_OUT_CMD_SWITCH_OFF        0x00U

#define CAN_TX_QUEUE_SIZE                 16U

/**
 * @brief CAN Frame structure
 * @param id: bit[0:28] is for CAN ID
 *            bit[29:30] is reserved
 *            bit[31] is for IDE
 * @param data: Data buffer (8 bytes)
 */
typedef struct {
  uint32_t id;
  uint8_t data[8];
} CAN_Frame;

typedef struct {
  CAN_TxHeaderTypeDef header;
  uint8_t data[8];
} CAN_TxFrame;

typedef struct {
  CAN_RxHeaderTypeDef header;
  uint8_t data[8];
} CAN_RxFrame;

/*
 * @brief Digital Output Command (VCU -> ECU)
 */
typedef struct
{
  uint8_t switchCmd     : SWITCH_CMD_BITS;
  uint8_t dutyCycle     : DUTY_CYCLE_BITS;
} DigitalOutput_Cmd;

typedef union {
  uint64_t sdu;
  DigitalOutput_Cmd signal[DIGITAL_OUT_CMD_SIGNAL_PER_FRAME];
} DigitalOutput_Cmd_Frame;

/*
 * @brief Digital Output Response (ECU -> VCU)
 */
typedef struct
{
  uint8_t statusPS      : STATUS_PS_BITS;
  uint8_t elDiagnosis   : OUTPUT_EL_DIAGNOSIS_BITS;
  uint16_t currentFB    : CURRENT_FB_BITS;
} DigitalOutput_Resp;

typedef union {
  uint64_t sdu;
  DigitalOutput_Resp signal[DIGITAL_OUT_RESP_SIGNAL_PER_FRAME];
} DigitalOutput_Resp_Frame;

/*
 * @brief Digital Input Response (ECU -> VCU)
 */
typedef struct
{
  uint8_t inputStatus   : INPUT_STATUS_BITS;
  uint8_t freshness     : FRESHNESS_BITS;
  uint8_t elDiagnosis   : INPUT_EL_DIAGNOSIS_BITS;
} DigitalInput_Resp;

typedef union {
  uint64_t sdu;
  DigitalInput_Resp signal[DIGITAL_IN_RESP_SIGNAL_PER_FRAME];
} DigitalInput_Resp_Frame;

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

/*
 * @brief Enqueue a CAN transmission frame
 * @param header: Pointer to the CAN header
 * @param data: Pointer to the data buffer
 * @retval true if successful, false if full
 */
bool CAN_EnqueueTxFrame(CAN_Frame *frame, CAN_Frame *canTxQueue);

/*
 * @brief Dequeue a CAN transmission frame
 * @param header: Pointer to the CAN header to fill
 * @param data: Pointer to the data buffer to fill
 * @retval true if successful, false if empty
 */
bool CAN_DequeueTxFrame(CAN_Frame *frame, CAN_Frame *canTxQueue);

/*
 * @brief Dequeue and send CAN transmission frames
 * @param hcan: Pointer to CAN handle
 * @param canTxQueue: Pointer to the CAN transmission queue
 */
void CAN_send(CAN_HandleTypeDef *hcan, CAN_Frame *canTxQueue);

/*
 * @brief Process received CAN command
 * @param frame: Pointer to CAN frame
 * @param cmd_frame: Pointer to command frames
 */
void CAN_process_command(CAN_RxFrame *frame, DigitalOutput_Cmd_Frame *cmd_frame);

#endif /* INC_CAN_H_ */
