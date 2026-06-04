# 5-DOF Robot Arm Control System

An academic and research-oriented repository demonstrating real-time control, system identification, and ROS 2 integration for a custom 5-Degree-of-Freedom (5-DOF) robotic manipulator. 

Developed as part of the 5XEB0 course, this project bridges low-level mechatronics with high-level robotic operating systems, showcasing precise motor control, trajectory generation, and modular software design.

## 🧠 High-Level Control Architecture

The system utilizes a distributed control architecture to separate high-frequency, real-time hardware execution from higher-level trajectory planning and communication.

* **Low-Level Control (Teensy 4.0):** Handles real-time execution. It reads encoder feedback, computes high-speed control loops (PD/PID position control), and commands the motors. Written in C++ using PlatformIO.
* **High-Level Control (Raspberry Pi & ROS 2):** Acts as the primary compute node. It handles ROS 2 message publishing/subscribing, trajectory generation, and GUI interactions.
* **Middleware (micro-ROS):** Bridges the hardware and software layers. The Teensy runs a micro-ROS client that communicates with the ROS 2 agent on the Raspberry Pi over UART, allowing seamless topic serialization (e.g., joint states, command velocities).
* **System Identification (MATLAB/Simulink):** Used extensively for logging step-response data, mapping the dynamic behavior of the 5-DOF system, and tuning the controller gains for smooth, precise actuation.

## 📁 Repository Structure

The codebase is highly modular, split into focused sub-projects for systematic testing and deployment:

* **`/System identification base` & `/Logged data`:** MATLAB scripts, Simulink models, and `.mat` data logs used to derive the mathematical model of the arm and tune the control loops.
* **PlatformIO Modules:**
  * `/Basic joint tests` & `/Encoder speed`: Low-level hardware verification and sensor calibration.
  * `/Base position control` & `/Jogger`: Closed-loop movement and manual articulation.
  * `/Single_joint_movement_ROS` & `/Dual_joint_movement_ROS`: micro-ROS implementations for single and multi-axis trajectory execution.
* **`/Reports & papers`:** Academic documentation, including the final peer-reviewed 5XEB0 paper detailing the system's design and performance metrics.
* **`/Plots`:** Visualizations of step responses, error tracking, and polynomial trajectory curves.

## 🚀 Setup & Execution Overview

Deploying the system requires a two-node configuration:

1. **Microcontroller Firmware:** Compile and flash the desired operational mode (e.g., `Dual_joint_movement_ROS`) onto the Teensy 4.0 using the PlatformIO IDE. 
2. **ROS 2 Agent:** Connect the Teensy to the Raspberry Pi via USB/UART. Initialize the micro-ROS agent on the Pi to establish the hardware bridge.
3. **Operation:** Once the agent connects, the Teensy becomes an active ROS 2 node, ready to receive trajectory commands and publish real-time encoder telemetry to the network.
