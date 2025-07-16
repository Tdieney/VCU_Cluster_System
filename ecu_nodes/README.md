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

