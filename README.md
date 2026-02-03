# Washing Machine Controller Simulation

## Introduction
This project simulates a washing machine controller using a PIC16F887 microcontroller. The system is designed for reliability and maintainability, featuring a Finite State Machine (FSM) architecture to manage washing cycles, user inputs, and safety mechanisms. The simulation is executed in Proteus, with firmware developed in MPLAB.

## Features
*   **Washing Modes:** Supports Normal, Quick, and Heavy wash profiles.
*   **Customizable Settings:** Users can adjust water levels (Low, Medium, High).
*   **Safety Interlocks:** Real-time door sensor monitoring using hardware interrupts to immediately halt operations if safety is compromised.
*   **State-of-the-Art Architecture:** Implements a non-blocking FSM for responsive user interface and robust process control.
*   **User Interface:** 16x2 LCD display provides status updates and menu navigation.

## Hardware Architecture
*   **Microcontroller:** PIC16F887
*   **Display:** 16x2 Character LCD (HD44780 controller compatible)
*   **Actuators:**
    *   DC Motor (Water Inlet)
    *   DC Motor (Drum Spin)
*   **Sensors & Inputs:**
    *   Door Sensor (Interrupt-driven)
    *   Menu Navigation Buttons (Menu, Select, Confirm, Start/Stop)

## Software Design
The firmware is written in C and compiled with the XC8 compiler. Key design patterns include:
*   **Finite State Machine (FSM):** The core logic transitions between distinct states (IDLE, FILLING, WASHING, RINSING, SPINNING, ERROR) based on sensor inputs and timers.
*   **Hardware Abstraction:** Hardware-specific pin definitions are separated from the application logic.
*   **Safety Critical Interrupts:** The door sensor triggers a high-priority interrupt to ensure immediate machine shutdown.

## Installation and Usage
1.  **Prerequisites:**
    *   MPLAB X IDE with XC8 Compiler.
    *   Proteus Design Suite.
2.  **Simulation:**
    *   Open `Schematic/Washing Machine.pdsprj` in Proteus.
    *   Ensure the PIC16F887 component is linked to the compiled `.hex` file.
    *   Run the simulation.
3.  **Operation:**
    *   Use the **Menu** button to toggle between Mode and Water Level settings.
    *   Use the **Select** button to cycle through options.
    *   Press **Confirm** to save settings.
    *   Press **Start/Stop** to begin or pause the cycle.

## License
This project is licensed under the MIT License. See the LICENSE file for details.
