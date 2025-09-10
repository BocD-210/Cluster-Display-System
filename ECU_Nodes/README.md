# Firmware ECU Nodes and Connection I/O
## Overview

In this Cluster Display System project, the implementation is purely a simulation; therefore, no actual ECU hardware is used. Instead, a firmware prototype is designed to simulate selected functionalities of real ECUs.

The firmware is developed on the STM32F103C8 microcontroller, emulating basic operations such as speed variation, battery level monitoring, and indicator signals. This is achieved using simple hardware components like potentiometers, encoders, LEDs, and buttons, while leveraging STM32 peripherals including GPIO, interrupts, timers, and ADCs. Furthermore, CAN communication is implemented to enable interaction with the VCU.

This approach allows students like me to practice and integrate an Embedded Linux system without the complexity of real automotive hardware, while still meeting the original project objectives.

## I/O Connection
The table below describes the connection diagram of ECU nodes simulated by STM32F1.

| **STM32 Pin** | **Function** | **I/O Connection**             |
|-------------- | ------------ | ------------------------------ |
| PA0           | GPIO_EXTI0   | BUTTON                         |
| PA1           | GPIO_EXTI1   | CLK (Encoder)                  |
| PA2           | GPIO_Input   | Data (Encoder)                 |
| PA3           | ADC1_IN3     | Potentiometer                  |
| PA4           | GPIO_EXTI4   | BUTTON                         |
| PA5           | GPIO_EXTI5   | BUTTON                         |
| PA11          | CAN_RX       |                                |
| PA12          | CAN_TX       |                                |
| PC13          | GPIO_Output  | LED in STM32                   |
| PA15          | TIM2_CH1     | LED (Control by Encoder)       |
| PB3           | TIM2_CH2     | LED (Control by Potentiometer) |
| PB6           | GPIO_Output  | turnright light                |
| PB7           | GPIO_Output  | park light                     |
| PB8           | GPIO_Output  | turnleft light                 |

## Illustrate hardware 

![Diagram](../Hardware.png)
