# ECU Nodes Firmware

This directory contains the firmware source code for the STM32F1-based Electronic Control Unit (ECU) nodes used in the VCU Cluster System project.

## Structure

- **Core/**: Main application code, including FreeRTOS tasks, CAN protocol handlers, and hardware abstraction.
- **Drivers/**: STM32 HAL and CMSIS drivers.
- **Middlewares/**: Third-party libraries such as FreeRTOS (CMSIS v2).

## Features

- CAN communication protocol for digital input/output control.
- FreeRTOS-based multitasking (Rx/Tx handlers).
- Modular and scalable code structure for automotive applications.

## I/O Connection

The table below describes the connection diagram of ECU nodes simulated by STM32F1.

| **STM32 Pin** | **Function** | **Signal Name** | **I/O Connection**              |
| ------------- | ------------ | --------------- | ------------------------------- |
| PA5           | ADC1_IN5     | -               | Potentiometer                   |
| PA6           | TIM3_CH1     | DIGITAL_OUT_8   | Buzzer                          |
| PA7           | TIM3_CH2     | DIGITAL_OUT_12  | Red LED                         |
| PB0           | TIM3_CH3     | DIGITAL_OUT_22  |                                 |
| PB1           | TIM3_CH4     | DIGITAL_OUT_32  |                                 |
| PA8           | TIM1_CH1     | DIGITAL_OUT_28  | Green LED (Right Rear Light)    |
| PA9           | TIM1_CH2     | DIGITAL_OUT_19  | Yellow LED (Right Front Light)  |
| PA10          | TIM1_CH3     | DIGITAL_OUT_10  | Yellow LED (Left Front Light)   |
| PA11          | TIM1_CH4     | DIGITAL_OUT_1   | Green LED (Left Rear Light)     |
| PB12          | GPIO_Output  | BTN_COL0        | Button matrix (column 1)        |
| PB13          | GPIO_Output  | BTN_COL1        | Button matrix (column 2)        |
| PB14          | GPIO_Output  | BTN_COL2        | Button matrix (column 3)        |
| PB15          | GPIO_Output  | BTN_COL3        | Button matrix (column 4)        |
| PB4           | GPIO_EXTI4   | BTN_ROW0        | Button matrix (row 1)           |
| PB5           | GPIO_EXTI5   | BTN_ROW1        | Button matrix (row 2)           |
| PB6           | GPIO_EXTI6   | BTN_ROW2        | Button matrix (row 3)           |
| PB7           | GPIO_EXTI7   | BTN_ROW3        | Button matrix (row 4)           |
| PB8           | CAN_RX       | -               | CAN transceiver (CRX)           |
| PB9           | CAN_TX       | -               | CAN transceiver (CTX)           |
