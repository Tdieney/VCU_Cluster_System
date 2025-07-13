/*
 * rb_can.c
 *
 *  Created on: Jul 12, 2025
 *      Author: Tdieney
 */

#include "rb_can.h"
#include <string.h>

#define IDE_BIT_POSITION  31U
#define BYTES_PER_FRAME   8U

static uint8_t canTxHead = 0, canTxTail = 0;

/*
 * @brief Enqueue a CAN transmission frame
 * @param frame: Pointer to the CAN frame
 * @param canTxQueue: Pointer to the CAN transmission queue
 * @retval true if successful, false if full
 */
bool rb_CAN_EnqueueTxFrame(rb_CAN_Frame *frame, rb_CAN_Frame *canTxQueue) {
  uint8_t next = (canTxHead + 1) % CAN_TX_QUEUE_SIZE;
  if (next == canTxTail) return false; // Queue full
  memcpy(&canTxQueue[canTxHead], frame, sizeof(rb_CAN_Frame));
  canTxHead = next;
  return true;
}

/*
 * @brief Dequeue a CAN transmission frame
 * @param frame: Pointer to the CAN frame to fill
 * @param canTxQueue: Pointer to the CAN transmission queue
 * @retval true if successful, false if empty
 */
bool rb_CAN_DequeueTxFrame(rb_CAN_Frame *frame, rb_CAN_Frame *canTxQueue) {
  if (canTxHead == canTxTail) return false; // Queue empty
  memcpy(frame, &canTxQueue[canTxTail], sizeof(rb_CAN_Frame));
  canTxTail = (canTxTail + 1) % CAN_TX_QUEUE_SIZE;
  return true;
}

/*
 * @brief Dequeue and send CAN transmission frames
 * @param hcan: Pointer to CAN handle
 * @param canTxQueue: Pointer to the CAN transmission queue
 */
void rb_CAN_send(CAN_HandleTypeDef *hcan, rb_CAN_Frame *canTxQueue)
{
  rb_CAN_Frame tx_frame;
  CAN_TxHeaderTypeDef tx_header;

  while (rb_CAN_DequeueTxFrame(&tx_frame, canTxQueue)) {
    uint32_t txMailbox;
    HAL_StatusTypeDef status = HAL_BUSY;

    do {
      if (tx_frame.id >> IDE_BIT_POSITION) {
        // Extended ID
        tx_header.IDE = CAN_ID_EXT;
        tx_header.ExtId = tx_frame.id & 0x1FFFFFFF; // Mask to 29 bits
      } else {
        // Standard ID
        tx_header.IDE = CAN_ID_STD;
        tx_header.StdId = tx_frame.id & 0x7FF; // Mask to 11 bits
        tx_header.ExtId = 0; // Not using extended ID
      }
      tx_header.RTR = CAN_RTR_DATA; // Data frame
      tx_header.DLC = BYTES_PER_FRAME; // Data length code

      status = HAL_CAN_AddTxMessage(hcan, &tx_header, tx_frame.data, &txMailbox);
    } while (status != HAL_OK);

    if (status != HAL_OK) {
      // Handle error
      Error_Handler();
    }
  }
}

/*
 * @brief Process received CAN command
 * @param frame: Pointer to CAN frame
 * @param cmd_frame: Pointer to command frames
 */
void rb_CAN_process_command(CAN_RxFrame *frame, rb_DigitalOutput_Cmd_Frame *cmd_frame)
{
  // Buffer to hold received data
  uint8_t data[BYTES_PER_FRAME];
  // Check if the received frame is a Digital Output Command
  if (frame->header.IDE == CAN_ID_EXT)
  {
    // Extract command data from the received frame
    for (uint8_t i = 0; i < NUMBER_OF_DIG_OUT_CMD_FRAME; i++)
    {
      if (DIGITAL_OUTPUT_CMD_ID(i) == frame->header.ExtId)
      {
        // Copy data to buffer
        for (uint8_t j = 0; j < BYTES_PER_FRAME; j++)
        {
          data[j] = frame->data[j];
        }
        cmd_frame->sdu = *((uint64_t *)data);
        break;
      }
    }
  }
}


