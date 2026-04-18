# Hexapod

## Overview

This is a personal project of a 6-legged (hexapod) robot using ESP32.
The robot is designed for learning embedded systems, robotics control, and mechanical design.

## Features

* Omnidirectional movement (move in all directions)
* Rotation in place (yaw control)
* Tripod gait walking
* Remote control via WiFi (ESP32 ↔ ESP32)
* Modular design (easy to upgrade)

## Hardware (Electronics)

* 2 × ESP32 (robot + controller)
* 2 × PCA9685 servo driver
* 18 × MG90S servo motors (18 DOF)
* 7.4V battery (30C)
* 5V step-down (buck converter)
* Joystick module (for controller)

## Mechanical Design

* 3D printed frame (SolidWorks design)

## Software

* Language: C / Arduino
* Platform: ESP32
* Communication: WiFi
* Control:
  Gait algorithm (tripod gait)
  Inverse kinematics (developing)

## Project Structure

* `Code/`:

  * `robot/`: firmware for hexapod
  * `control/`: code for joystick controller
* `hardware/`: SolidWorks files 
