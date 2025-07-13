/*
 * rb_can.h
 *
 *  Created on: Jul 12, 2025
 *      Author: Tdieney
 */

#ifndef INC_RB_CAN_H_
#define INC_RB_CAN_H_

#include <stdint.h>
#include <stdbool.h>
#include "main.h"

// CAN IDs
#define DIGITAL_OUTPUT_CMD_ID(n) (0x94FF0000UL + ((n) * 0x20UL))
#define DIGITAL_OUTPUT_RES_ID(n) (0x94FF0800UL + ((n) * 0x20UL))
#define DIGITAL_INPUT_RES_ID(n)  (0x94FF0A00UL + ((n) * 0x20UL))

#define NUMBER_OF_DIG_OUT_CMD_FRAME 4U
#define NUMBER_OF_DIG_OUT_RES_FRAME 8U
#define NUMBER_OF_DIG_IN_RES_FRAME  4U

// CAN Frame Signal Counts
#define RB_DIGITAL_OUT_CMD_SIGNAL_COUNT   8U
#define RB_DIGITAL_OUT_RESP_SIGNAL_COUNT   4U
#define RB_DIGITAL_IN_RESP_SIGNAL_COUNT    8U

// Bitfield widths for Digital Output/Tx/Inputs
#define RB_SWITCH_CMD_BITS            1U
#define RB_DUTY_CYCLE_BITS            7U
#define RB_STATUS_PS_BITS             1U
#define RB_OUTPUT_EL_DIAGNOSIS_BITS   3U
#define RB_CURRENT_FB_BITS            12U
#define RB_INPUT_STATUS_BITS          1U
#define RB_FRESHNESS_BITS             5U
#define RB_INPUT_EL_DIAGNOSIS_BITS    2U

#define CAN_TX_QUEUE_SIZE 16U

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
} rb_CAN_Frame;

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
  uint8_t switchCmd     : RB_SWITCH_CMD_BITS;
  uint8_t dutyCycle     : RB_DUTY_CYCLE_BITS;
} rb_DigitalOutput_Cmd;

typedef union {
  uint64_t sdu;
  rb_DigitalOutput_Cmd signal[RB_DIGITAL_OUT_CMD_SIGNAL_COUNT];
} rb_DigitalOutput_Cmd_Frame;

/*
 * @brief Digital Output Response (ECU -> VCU)
 */
typedef struct
{
  uint8_t statusPS      : RB_STATUS_PS_BITS;
  uint8_t elDiagnosis   : RB_OUTPUT_EL_DIAGNOSIS_BITS;
  uint16_t currentFB    : RB_CURRENT_FB_BITS;
} rb_DigitalOutput_Resp;

typedef union {
  uint64_t sdu;
  rb_DigitalOutput_Resp signal[RB_DIGITAL_OUT_RESP_SIGNAL_COUNT];
} rb_DigitalOutput_Resp_Frame;

/*
 * @brief Digital Input Response (ECU -> VCU)
 */
typedef struct
{
  uint8_t inputStatus   : RB_INPUT_STATUS_BITS;
  uint8_t freshness     : RB_FRESHNESS_BITS;
  uint8_t elDiagnosis   : RB_INPUT_EL_DIAGNOSIS_BITS;
} rb_DigitalInput_Resp;

typedef union {
  uint64_t sdu;
  rb_DigitalInput_Resp signal[RB_DIGITAL_IN_RESP_SIGNAL_COUNT];
} rb_DigitalInput_Resp_Frame;

/*
 * @brief Enqueue a CAN transmission frame
 * @param header: Pointer to the CAN header
 * @param data: Pointer to the data buffer
 * @retval true if successful, false if full
 */
bool rb_CAN_EnqueueTxFrame(rb_CAN_Frame *frame, rb_CAN_Frame *canTxQueue);

/*
 * @brief Dequeue a CAN transmission frame
 * @param header: Pointer to the CAN header to fill
 * @param data: Pointer to the data buffer to fill
 * @retval true if successful, false if empty
 */
bool rb_CAN_DequeueTxFrame(rb_CAN_Frame *frame, rb_CAN_Frame *canTxQueue);

/*
 * @brief Dequeue and send CAN transmission frames
 * @param hcan: Pointer to CAN handle
 * @param canTxQueue: Pointer to the CAN transmission queue
 */
void rb_CAN_send(CAN_HandleTypeDef *hcan, rb_CAN_Frame *canTxQueue);

/*
 * @brief Process received CAN command
 * @param frame: Pointer to CAN frame
 * @param cmd_frame: Pointer to command frames
 */
void rb_CAN_process_command(CAN_RxFrame *frame, rb_DigitalOutput_Cmd_Frame *cmd_frame);

#endif /* INC_RB_CAN_H_ */
