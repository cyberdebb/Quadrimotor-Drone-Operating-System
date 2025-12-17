# Quadrimotor Drone Operating System

A custom real-time operating system designed for controlling quadrotor drones with multi-threaded task management, motor control, and sensor integration.

---

## About the Project

This project implements a complete operating system kernel specifically designed for quadrotor drone control. It features a real-time scheduler, mutex-based synchronization, hardware abstraction layer, and integrated support for accelerometers, gyroscopes, and drone motor control. The system manages four DC motors with PWM control and processes sensor data from inertial measurement units.

---

## Technologies Used

- C programming language
- Real-time kernel design
- PIC18F4550 microcontroller
- IRF540 MOSFETs for motor control
- DS18B20 temperature sensors
- Accelerometer and gyroscope sensors
- Custom task scheduler
- Makefile build system

---

## Hardware Components

- PIC18F4550 microcontroller (U1)
- Four IRF540 N-channel MOSFETs (Q1-Q4) for motor driving
- Four DC motors (M1-M4) with 12V power supply
- Accelerometer sensor
- Gyroscope sensor
- Temperature sensors
- Resistor network for signal conditioning
- Pull-up resistors for sensor interfaces

---

## Project Structure

```
Quadrimotor-Drone-Operating-System/
├── Simulacao/              # Simulation and testing code
├── nbproject/              # NetBeans project configuration
├── kernel.c                # Kernel implementation
├── kernel.h                # Kernel definitions
├── scheduler.c             # Task scheduler implementation
├── scheduler.h             # Scheduler definitions
├── hardware.c              # Hardware abstraction layer
├── hardware.h              # Hardware definitions
├── mutex.c                 # Mutex synchronization
├── mutex.h                 # Mutex definitions
├── syscall.c               # System calls
├── syscall.h               # System call definitions
├── user_app.c              # User application code
├── user_app.h              # User application definitions
├── types.h                 # Type definitions
├── os_config.h             # OS configuration
├── main.c                  # Main entry point
├── Makefile                # Build configuration
└── .gitignore              # Git ignore file
```

---

## Features

- Real-time multitasking kernel with preemptive scheduling
- Task synchronization using mutexes
- Hardware abstraction layer for sensor and motor control
- PWM-based motor speed control
- Sensor data acquisition and processing
- System call interface for user applications
- Configurable task priorities and time slices
- Support for multiple concurrent sensor readings

---

## Building the Project

The project was developed using MPLAB IDE v8.35 and tested with Proteus 8 Professional simulation.

### Building with MPLAB IDE 8.35

1. Open MPLAB IDE v8.35
2. Open the project file in the root directory
3. Configure the project settings:
   - Select Device: PIC18F4550
   - Select Compiler: MPLAB C18
4. Build the project:
   - Project > Build All
   - Or use keyboard shortcut Ctrl+F10

5. The compiled hex file will be generated in the project output directory

### Alternative: Using Makefile

If using command-line build:
```bash
make
```

Clean build artifacts:
```bash
make clean
```

### Simulation with Proteus 8 Professional

The project includes a Proteus 8 Professional simulation schematic for hardware-in-the-loop testing:

1. Open Proteus 8 Professional
2. Load the simulation schematic containing:
   - PIC18F4550 microcontroller
   - Four DC motor circuits with IRF540 MOSFETs
   - Sensor interfaces (accelerometer, gyroscope)
   - Power supply configuration (12V for motors)
3. Configure the microcontroller model:
   - Model: PIC18F4550
   - Load the compiled hex file from MPLAB
4. Run the simulation to verify:
   - Motor control behavior
   - Sensor data processing
   - Real-time task scheduling
   - System responsiveness

This hardware-in-the-loop simulation approach allows thorough testing before actual hardware deployment.

---

## Motor Control

The system controls four DC motors using IRF540 MOSFET drivers:

- Motor 1 (M1): Controlled by Q1 via GPIO M1
- Motor 2 (M2): Controlled by Q2 via GPIO M2
- Motor 3 (M3): Controlled by Q3 via GPIO M3
- Motor 4 (M4): Controlled by Q4 via GPIO M4

Each motor receives PWM signals through 100-ohm gate resistors with 10k-ohm pull-down resistors for safety.

---

## Sensor Integration

The system reads data from multiple sensors connected to the PIC18F4550:

- Accelerometer: 3-axis acceleration measurement
- Gyroscope: 3-axis angular velocity measurement
- Temperature Sensors: Thermal monitoring via DS18B20 protocol

All sensor data is processed through the kernel's real-time scheduler for consistent timing.

---

## Configuration

System configuration is managed through `os_config.h`. Key configurable parameters include:

- Task queue size
- Maximum number of tasks
- Scheduler time slice duration
- Mutex pool size
- System clock frequency
- Hardware pin assignments

---

## System Calls

The operating system provides system call interface for user applications:

- Task creation and management
- Mutex operations (lock/unlock)
- Sensor data reading
- Motor control commands
- Timing and delay functions

---

## Simulation

The project includes a simulation folder for testing kernel behavior and task scheduling before hardware deployment. Use the simulation code to verify:

- Task scheduling logic
- Mutex synchronization
- Sensor data processing
- Motor control algorithms

### Schematic Capture

![aaa_page-0001](https://github.com/user-attachments/assets/8d26df8e-8c5a-4509-9564-7dbfdb60fdc9)

