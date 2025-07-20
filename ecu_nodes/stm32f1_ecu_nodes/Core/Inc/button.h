/*
 * button.h
 *
 *  Created on: Jul 20, 2025
 *      Author: Tdieney
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "main.h"
#include <stdbool.h>
#include "cmsis_os2.h"
#include "can.h"

#define DEBOUNCE_BTN_MS         20U
#define NUMBER_OF_BUTTONS       16U
#define NUMBER_OF_BTN_COLUMNS   4U
#define NUMBER_OF_BTN_ROWS      4U

/*
 * @brief  Initialize the button matrix
 */
void btn_matrix_init(void);

/*
 * @brief  Check if a button is still pressed
 * @param  row: The row of the button
 * @param  col: The column of the button
 * @retval true if the button is still pressed, false otherwise
 */
bool is_btn_still_pressed(uint8_t row, uint8_t col);

/*
 * @brief  Turn on a specific column in the button matrix
 * @param  col: The column to turn on
 */
void turn_btn_matrix_col(uint8_t col);

/*
 * @brief  Process button press
 * @param  btn_state: Pointer to the button state array
 * @param  digital_input_data: Pointer to the Digital Input Response Frame
 *                              to update input states
 */
void process_btn_pressed(uint8_t *btn_state, DigitalInput_Resp_Frame *digital_input_data);

/*
 * @brief  Scan the button matrix
 * @param  digital_input_data: Pointer to the Digital Input Response Frame
 *                              to update input states
 */
void scan_btn_matrix(DigitalInput_Resp_Frame *digital_input_data);

/*
 * @brief  Process the button matrix
 * @param  digital_input_data: Pointer to the Digital Input Response Frame
 *                              to update input states
 */
void btn_matrix_process(DigitalInput_Resp_Frame *digital_input_data);

#endif /* INC_BUTTON_H_ */
