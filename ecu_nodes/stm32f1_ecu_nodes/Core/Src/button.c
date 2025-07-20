/*
 * button.c
 *
 *  Created on: Jul 20, 2025
 *      Author: Tdieney
 */

#include "button.h"

GPIO_TypeDef* row_ports[NUMBER_OF_BTN_ROWS] = {BTN_ROW0_GPIO_Port, BTN_ROW1_GPIO_Port, BTN_ROW2_GPIO_Port, BTN_ROW3_GPIO_Port};
uint16_t row_pins[NUMBER_OF_BTN_ROWS]       = {BTN_ROW0_Pin, BTN_ROW1_Pin, BTN_ROW2_Pin, BTN_ROW3_Pin};

GPIO_TypeDef* col_ports[NUMBER_OF_BTN_COLUMNS] = {BTN_COL0_GPIO_Port, BTN_COL1_GPIO_Port, BTN_COL2_GPIO_Port, BTN_COL3_GPIO_Port};
uint16_t col_pins[NUMBER_OF_BTN_COLUMNS]       = {BTN_COL0_Pin, BTN_COL1_Pin, BTN_COL2_Pin, BTN_COL3_Pin};

volatile uint8_t is_debouncing = 0;
volatile uint8_t btn_still_pressed = 0;
volatile uint32_t debounce_tick = 0;
volatile uint8_t last_row = 0;
volatile uint8_t last_col = 0;
volatile uint8_t btn_state[MAX_DIG_IN_RESP_SIGNALS] = {0};

/*
 * @brief  EXTI line detection callback
 * @param  GPIO_Pin: Specifies the pin connected to the EXTI line
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (is_debouncing) return;

  for (int row = 0; row < NUMBER_OF_BTN_ROWS; row++) {
    if (GPIO_Pin == row_pins[row]) {
      is_debouncing = true;
      debounce_tick = HAL_GetTick();
      last_row = row;

      // Disable EXTI line temporarily
      switch (row) {
        case 0:
          HAL_NVIC_DisableIRQ(EXTI4_IRQn);
          break;
        case 1:
        case 2:
        case 3:
          HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
          break;
      }
      break;
    }
  }
}

/*
 * @brief  Initialize the button matrix
 */
void btn_matrix_init(void) {
  // Set all COLs to HIGH
  for (int i = 0; i < NUMBER_OF_BTN_COLUMNS; i++) {
    col_ports[i]->ODR |= col_pins[i];
  }
}

/*
 * @brief  Check if a button is still pressed
 * @param  row: The row of the button
 * @param  col: The column of the button
 * @retval true if the button is still pressed, false otherwise
 */
bool is_btn_still_pressed(uint8_t row, uint8_t col) {
  // Set all COLs to HIGH
  for (int i = 0; i < NUMBER_OF_BTN_COLUMNS; i++) {
    col_ports[i]->ODR |= col_pins[i];
  }

  // Set the specific COL to LOW
  col_ports[col]->ODR &= ~col_pins[col];
  osDelay(1);

  return ((row_ports[row]->IDR & row_pins[row]) == GPIO_PIN_RESET);
}

/*
 * @brief  Turn on a specific column in the button matrix
 * @param  col: The column to turn on
 */
void turn_btn_matrix_col(uint8_t col) {
  // Set all COLs to HIGH
  for (int i = 0; i < NUMBER_OF_BTN_COLUMNS; i++) {
    col_ports[i]->ODR |= col_pins[i];
  }

  // Set the specific COL to LOW
  col_ports[col]->ODR &= ~col_pins[col];
  osDelay(1);
}

/*
 * @brief  Process button press
 * @param  btn_state: Pointer to the button state array
 * @param  digital_input_data: Pointer to the Digital Input Response Frame
 *                              to update input states
 */
void process_btn_pressed(uint8_t *btn_state, DigitalInput_Resp_Frame *digital_input_data) {
  GPIOC->ODR ^= GPIO_PIN_13; // Toggle PC13 LED

  for (uint8_t i = 0; i < NUMBER_OF_DIG_IN_RES_FRAME; i++) {
    for (uint8_t j = 0; j < DIGITAL_IN_RESP_SIGNAL_PER_FRAME; j++) {
      digital_input_data[i].signal[j].inputStatus = btn_state[i * DIGITAL_IN_RESP_SIGNAL_PER_FRAME + j] & 0x01;
    }
  }
}

/*
 * @brief  Scan the button matrix
 * @param  digital_input_data: Pointer to the Digital Input Response Frame
 *                              to update input states
 */
void scan_btn_matrix(DigitalInput_Resp_Frame *digital_input_data) {
  static uint8_t btn_idx = 0;
  for (int col = 0; col < NUMBER_OF_BTN_COLUMNS; col++) {
    // Set all COLs to HIGH
    for (int i = 0; i < NUMBER_OF_BTN_COLUMNS; i++) {
      col_ports[i]->ODR |= col_pins[i];
    }

    // Set the specific COL to LOW
    col_ports[col]->ODR &= ~col_pins[col];
    osDelay(1);

    for (int row = 0; row < NUMBER_OF_BTN_ROWS; row++) {
      if ((row_ports[row]->IDR & row_pins[row]) == GPIO_PIN_RESET) {
        // Detected a button press
        last_col = col;
        btn_idx = row * 9U + col;
        // Toggle button state
        btn_state[btn_idx] = !btn_state[btn_idx];
        process_btn_pressed(btn_state, digital_input_data);
        return;
      }
    }
  }
}

/*
 * @brief  Process the button matrix
 * @param  digital_input_data: Pointer to the Digital Input Response Frame
 *                              to update input states
 */
void btn_matrix_process(DigitalInput_Resp_Frame *digital_input_data) {
  static uint8_t current_scan_col = 0;
  turn_btn_matrix_col(current_scan_col);
  current_scan_col = (current_scan_col + 1) % NUMBER_OF_BTN_COLUMNS;

  if (is_debouncing && (HAL_GetTick() - debounce_tick >= DEBOUNCE_BTN_MS)) {
    scan_btn_matrix(digital_input_data);
    is_debouncing = false;
    btn_still_pressed = true;
  }

  if (btn_still_pressed) {
    if (!is_btn_still_pressed(last_row, last_col)) {
      btn_still_pressed = false;

      // Re-enable EXTI
      switch (last_row) {
        case 0: 
          HAL_NVIC_EnableIRQ(EXTI4_IRQn);
          break;
        case 1:
        case 2:
        case 3:
          HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
          break;
      }
    }
  }
}


